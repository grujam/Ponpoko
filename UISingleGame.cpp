#include "stdafx.h"
#include "UISingleGame.h"
#include <string>

using namespace Ponpoko;

CUISingleGame::CUISingleGame()
{
    m_pMapTexture = GET_DX_MGR()->CreateSolidTexture(10, 10, D3DCOLOR_ARGB(255, 255, 255, 0));
    m_pPlayerTexture = GET_DX_MGR()->CreateSolidTexture(20, 30, D3DCOLOR_ARGB(255, 80, 200, 255));
    StartLoadingLevel(1);
}

CUISingleGame::~CUISingleGame()
{
    if (m_pMapTexture) m_pMapTexture->Release();
    if (m_pPlayerTexture) m_pPlayerTexture->Release();
}

void CUISingleGame::StartLoadingLevel(int nLevel)
{
    m_nCurrentLevel = nLevel;
    m_bLoading = true;
    m_fLoadingTimer = 0.0f;
}

void CUISingleGame::OnUpdate(float fDelta)
{
    if (m_bLoading)
    {
        m_fLoadingTimer += fDelta;
        if (m_fLoadingTimer < m_fLoadingDuration)
            return;

        std::wstring path = L"stage" + std::to_wstring(m_nCurrentLevel) + L".map";
        m_MapData.LoadMapData(path);
        m_Player.Reset(m_MapData.StartPos, m_MapData.GetRowCount());

        m_bLoading = false;
        return;
    }

    m_Player.Update(fDelta, m_MapData);

    if (m_MapData.Items.empty())
    {
        if (m_nCurrentLevel == 1)
            StartLoadingLevel(2);
        else
            GET_PPK_MGR()->EnterLobby();
    }
}

void CUISingleGame::OnRender()
{
    if (m_bLoading)
    {
        DrawLoadingScreen();
        return;
    }

    m_MapData.DrawMapData(m_pMapTexture);
    DrawPlayer();
}

void CUISingleGame::DrawPlayer()
{
    float fY = m_MapData.GetRowY(m_Player.GetRow());
    GET_DX_MGR()->DrawTexture(m_pPlayerTexture, m_Player.GetX() - 10.0f, fY - 30.0f - m_Player.GetJumpOffsetY());
}

void CUISingleGame::DrawLoadingScreen()
{
    RECT rect;
    GetClientRect(GET_DX_MGR()->GetHWND(), &rect);
    GET_DX_MGR()->DrawTextBlock(L"Loading Map...", rect, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CUISingleGame::OnKeyDown(WPARAM key)
{
    if (m_bLoading)
        return;

    switch (key)
    {
    case VK_LEFT:  m_Player.ApplyInput(EInputType::MoveLeft, m_MapData);  break;
    case VK_RIGHT: m_Player.ApplyInput(EInputType::MoveRight, m_MapData); break;
    case VK_UP:    m_Player.ApplyInput(EInputType::ClimbUp, m_MapData);   break;
    case VK_DOWN:  m_Player.ApplyInput(EInputType::ClimbDown, m_MapData); break;
    case VK_SPACE: m_Player.ApplyInput(EInputType::Jump, m_MapData);      break;
    }
}