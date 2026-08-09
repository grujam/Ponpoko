#include "stdafx.h"
#include "DXManager.h"
#include "Render.h"

bool CDXManager::Initialize(HWND hWnd, UINT width, UINT height)
{
    m_hWnd = hWnd;
    m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (m_pD3D == nullptr)
        return false;

    ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));
    m_d3dpp.Windowed = TRUE;
    m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    m_d3dpp.BackBufferWidth = width;
    m_d3dpp.BackBufferHeight = height;
    m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

    if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_d3dpp, &m_pDevice)))
        return false;

    if (FAILED(D3DXCreateSprite(m_pDevice, &m_pSprite)))
        return false;

    if (FAILED(D3DXCreateFontW(m_pDevice, 24, 0, FW_NORMAL, 1, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"맑은 고딕", &m_pFont)))
        return false;

    RECT clientRect;
    GetClientRect(m_hWnd, &clientRect);
    int nWidth = clientRect.right - clientRect.left;

    m_pFloorCeilingTexture = CreateSolidTexture(nWidth, 8, D3DCOLOR_ARGB(255, 40, 100, 255));
    m_pRowGuideTexture = CreateSolidTexture(nWidth, 2, D3DCOLOR_ARGB(120, 40, 100, 255));
    m_pLadderTexture = GET_DX_MGR()->LoadTexture(L"ladder.png");
    m_pSpikeTexture = GET_DX_MGR()->LoadTexture(L"spike.png");
    m_pStartTexture = CreateSolidTexture(20, 20, D3DCOLOR_ARGB(255, 255, 0, 0));
    m_pItem1Texture = GET_DX_MGR()->LoadTexture(L"item1.png");
    m_pItem2Texture = GET_DX_MGR()->LoadTexture(L"item2.png");
    m_pCharacterTexture = GET_DX_MGR()->LoadTexture(L"character.png");
    m_pEnemyTexture = GET_DX_MGR()->LoadTexture(L"enemy.png");

    return true;
}

void CDXManager::Delete()
{
    m_vRenderTargets.clear();

    if (m_pFont) { m_pFont->Release();   m_pFont = nullptr; }
    if (m_pSprite) { m_pSprite->Release(); m_pSprite = nullptr; }
    if (m_pDevice) { m_pDevice->Release(); m_pDevice = nullptr; }
    if (m_pD3D) { m_pD3D->Release();    m_pD3D = nullptr; }
}

void CDXManager::OnUpdate(float fDelta)
{
    BeginRender();
    EndRender();
}

void CDXManager::AddRenderTargets(std::shared_ptr<IRenderable> InRenderable)
{
	m_vRenderTargets.emplace_back(InRenderable);
}

void CDXManager::RemoveRenderTargets(std::shared_ptr<IRenderable> InRenderable)
{
	auto iter = std::find(m_vRenderTargets.begin(), m_vRenderTargets.end(), InRenderable);
	if (iter != m_vRenderTargets.end())
		m_vRenderTargets.erase(iter);
}

void CDXManager::BeginRender()
{
    m_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
    m_pDevice->BeginScene();
    m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

    for (const auto& target : m_vRenderTargets)
        target->OnRender();
}

void CDXManager::EndRender()
{
    m_pSprite->End();
    m_pDevice->EndScene();
    m_pDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

IDirect3DTexture9* CDXManager::LoadTexture(const wchar_t* path)
{
    IDirect3DTexture9* pTexture = nullptr;
    HRESULT hr = D3DXCreateTextureFromFileEx(
        m_pDevice, path,
        D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
        D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT,
        0, nullptr, nullptr, &pTexture);

    return SUCCEEDED(hr) ? pTexture : nullptr;
}

IDirect3DTexture9* CDXManager::CreateSolidTexture(UINT width, UINT height, D3DCOLOR color)
{
    IDirect3DTexture9* pTexture = nullptr;
    if (FAILED(m_pDevice->CreateTexture(width, height, 1, 0, D3DFMT_A8R8G8B8,
        D3DPOOL_MANAGED, &pTexture, nullptr)))
        return nullptr;

    D3DLOCKED_RECT rect;
    pTexture->LockRect(0, &rect, nullptr, 0);
    for (UINT y = 0; y < height; ++y)
    {
        DWORD* row = (DWORD*)((BYTE*)rect.pBits + y * rect.Pitch);
        for (UINT x = 0; x < width; ++x)
            row[x] = color;
    }
    pTexture->UnlockRect(0);

    return pTexture;
}

void CDXManager::DrawTexture(IDirect3DTexture9* pTexture, float x, float y, int alpha /* = 255 */, float fWidth /*= 0.0f*/, float fHeight /*= 0.0f*/)
{
    if (fWidth <= 0.0f && fHeight <= 0.0f)
    {
        D3DXVECTOR3 pos(x, y, 0.0f);
        m_pSprite->Draw(pTexture, nullptr, nullptr, &pos, D3DCOLOR_ARGB(alpha, 255, 255, 255));
        return;
    }

    D3DSURFACE_DESC desc;
    pTexture->GetLevelDesc(0, &desc);
    float fScaleX = (fWidth > 0.0f) ? fWidth / (float)desc.Width : 1.0f;
    float fScaleY = (fHeight > 0.0f) ? fHeight / (float)desc.Height : 1.0f;

    D3DXMATRIX matScale, matTrans, matTransform, matIdentity;
    D3DXMatrixScaling(&matScale, fScaleX, fScaleY, 1.0f);
    D3DXMatrixTranslation(&matTrans, x, y, 0.0f);
    matTransform = matScale * matTrans;

    m_pSprite->SetTransform(&matTransform);
    D3DXVECTOR3 zero(0.0f, 0.0f, 0.0f);
    m_pSprite->Draw(pTexture, nullptr, nullptr, &zero, D3DCOLOR_ARGB(alpha, 255, 255, 255));

    D3DXMatrixIdentity(&matIdentity);
    m_pSprite->SetTransform(&matIdentity);
}

void CDXManager::DrawTextBlock(const wchar_t* text, RECT rect, D3DCOLOR color)
{
    m_pFont->DrawTextW(m_pSprite, text, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE, color);
}
