#pragma once
#include "Singleton.h"
#include <d3d9.h>
#include <vector>
#include <memory>
#include <d3dx9.h>
#include <string>

#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")

class IRenderable;

class CDXManager : public ISingleton<CDXManager>
{
public:
	CDXManager() {}
    virtual ~CDXManager() { Delete(); }

public:
    bool Initialize(HWND hWnd, UINT width, UINT height);
    void Delete();
    void OnUpdate(float fDelta);

    void AddRenderTargets(std::shared_ptr<IRenderable> InRenderable);
    void RemoveRenderTargets(std::shared_ptr<IRenderable> InRenderable);

    void BeginRender();
    void EndRender();

public:
    IDirect3DDevice9* GetDevice() const { return m_pDevice; }
    const HWND& GetHWND() const { return m_hWnd; }

    IDirect3DTexture9* LoadTexture(const wchar_t* path);
    IDirect3DTexture9* CreateSolidTexture(UINT width, UINT height, D3DCOLOR color);

    IDirect3DTexture9* GetFloorCeilingTexture() { return m_pFloorCeilingTexture; }
    IDirect3DTexture9* GetRowGuideTexture() { return m_pRowGuideTexture; }
    IDirect3DTexture9* GetLadderTexture() { return m_pLadderTexture; }
    IDirect3DTexture9* GetSpikeTexture() { return m_pSpikeTexture; }
    IDirect3DTexture9* GetStartTexture() { return m_pStartTexture; }
    IDirect3DTexture9* GetItem1Texture() { return m_pItem1Texture; }
    IDirect3DTexture9* GetItem2Texture() { return m_pItem2Texture; }
    IDirect3DTexture9* GetCharacterTexture() { return m_pCharacterTexture; }
    IDirect3DTexture9* GetEnemyTexture() { return m_pEnemyTexture; }

    void DrawTexture(IDirect3DTexture9* pTexture, float x, float y, int alpha = 255, float fWidth = 0.0f, float fHeight = 0.0f);
    void DrawTextBlock(const wchar_t* text, RECT rect, D3DCOLOR color);

private:
    IDirect3D9* m_pD3D = nullptr;
    IDirect3DDevice9* m_pDevice = nullptr;
    ID3DXSprite* m_pSprite = nullptr;
    ID3DXFont* m_pFont = nullptr;

    D3DPRESENT_PARAMETERS m_d3dpp = {};
    HWND m_hWnd = nullptr;

    IDirect3DTexture9* m_pFloorCeilingTexture = nullptr;
    IDirect3DTexture9* m_pRowGuideTexture = nullptr;
    IDirect3DTexture9* m_pLadderTexture = nullptr;
    IDirect3DTexture9* m_pSpikeTexture = nullptr;
    IDirect3DTexture9* m_pStartTexture = nullptr;
    IDirect3DTexture9* m_pItem1Texture = nullptr;
    IDirect3DTexture9* m_pItem2Texture = nullptr;
    IDirect3DTexture9* m_pCharacterTexture = nullptr;
    IDirect3DTexture9* m_pEnemyTexture = nullptr;

    std::vector<std::shared_ptr<IRenderable>> m_vRenderTargets;
};
