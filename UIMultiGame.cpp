#include "stdafx.h"
#include "UIMultiGame.h"

using namespace Ponpoko;

CUIMultiGame::CUIMultiGame(bool bIsHost, const std::string& InJoinIP)
    : m_bIsHost(bIsHost)
{
    m_pMapTexture = GET_DX_MGR()->CreateSolidTexture(10, 10, D3DCOLOR_ARGB(255, 255, 255, 0));
    m_pLocalPlayerTexture = GET_DX_MGR()->CreateSolidTexture(20, 30, D3DCOLOR_ARGB(255, 80, 200, 255));
    m_pRemotePlayerTexture = GET_DX_MGR()->CreateSolidTexture(20, 30, D3DCOLOR_ARGB(255, 255, 120, 80));

    if (bIsHost)
        CNetworkManager::Get()->StartHost(m_nPort);
    else
        CNetworkManager::Get()->ConnectToHost(InJoinIP, m_nPort);
}

CUIMultiGame::~CUIMultiGame()
{
    if (m_pMapTexture) 
        m_pMapTexture->Release();
    if (m_pLocalPlayerTexture) 
        m_pLocalPlayerTexture->Release();
    if (m_pRemotePlayerTexture) 
        m_pRemotePlayerTexture->Release();
}

void CUIMultiGame::StartLoadingLevel(int nLevel)
{
    m_nCurrentLevel = nLevel;
    m_bLoadingLevel = true;
    m_fLoadingTimer = 0.0f;
}

void CUIMultiGame::OnUpdate(float fDelta)
{
    CNetworkManager::Get()->Update();

    if (CNetworkManager::Get()->IsFailed())
    {
        GET_PPK_MGR()->EnterLobby();
        return;
    }

    if (!CNetworkManager::Get()->IsConnected())
        return;

    if (m_bLoadingLevel)
    {
        m_fLoadingTimer += fDelta;
        if (m_fLoadingTimer < m_fLoadingDuration)
            return;

        std::wstring path = L"stage" + std::to_wstring(m_nCurrentLevel) + L".map";
        m_MapData.LoadMapData(path);

        int nStartRow = m_MapData.GetRowCount();
        m_LocalSession.Reset(m_MapData.StartPos, nStartRow);
        m_RemoteSession.Reset(m_MapData.StartPos, nStartRow);

        m_bLoadingLevel = false;
        m_nCurrentTick = 0;
        m_bTickSent = false;
        return;
    }

    m_fTickAccumulator += fDelta;
    while (m_fTickAccumulator >= m_fTickDuration)
    {
        LockStepTick();
        m_fTickAccumulator -= m_fTickDuration;
    }
}

void CUIMultiGame::LockStepTick()
{
    if (!m_bTickSent)
    {
        m_LastLocalPacket = SPonpokoPacket{};
        m_LastLocalPacket.nTick = m_nCurrentTick;
        m_LastLocalPacket.nActionCount = (int)m_PendingActions.size();
        for (size_t i = 0; i < m_PendingActions.size() && i < 4; ++i)
            m_LastLocalPacket.Actions[i] = m_PendingActions[i];
        m_PendingActions.clear();

        CNetworkManager::Get()->SendInput(m_LastLocalPacket);
        m_bTickSent = true;
    }

    SPonpokoPacket remotePacket;
    if (!CNetworkManager::Get()->TryPopRemoteInput(remotePacket))
        return;

    for (int i = 0; i < m_LastLocalPacket.nActionCount; ++i)
        m_LocalSession.ApplyInput(m_LastLocalPacket.Actions[i], m_MapData);

    for (int i = 0; i < remotePacket.nActionCount; ++i)
        m_RemoteSession.ApplyInput(remotePacket.Actions[i], m_MapData);

    ++m_nCurrentTick;
    m_bTickSent = false;

    if (m_MapData.Items.empty())
    {
        if (m_nCurrentLevel == 1)
            StartLoadingLevel(2);
        else
            GET_PPK_MGR()->EnterLobby();
    }
}

void CUIMultiGame::OnRender()
{
    if (!CNetworkManager::Get()->IsConnected())
    {
        DrawLoadingScreen(m_bIsHost ? L"상대방을 기다리는 중..." : L"호스트에 접속 중...");
        return;
    }

    if (m_bLoadingLevel)
    {
        DrawLoadingScreen(L"Loading Map...");
        return;
    }

    m_MapData.DrawMapData(m_pMapTexture);
    DrawPlayer(m_LocalSession, m_pLocalPlayerTexture);
    DrawPlayer(m_RemoteSession, m_pRemotePlayerTexture);
}

void CUIMultiGame::DrawPlayer(const CPlayerSession& InPlayer, IDirect3DTexture9* pTexture)
{
    float fY = m_MapData.GetRowY(InPlayer.GetRow());
    GET_DX_MGR()->DrawTexture(pTexture, InPlayer.GetX() - 10.0f, fY - 30.0f);
}

void CUIMultiGame::DrawLoadingScreen(const wchar_t* InText)
{
    RECT rect;
    GetClientRect(GET_DX_MGR()->GetHWND(), &rect);
    GET_DX_MGR()->DrawTextBlock(InText, rect, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CUIMultiGame::OnKeyDown(WPARAM key)
{
    if (m_bLoadingLevel || !CNetworkManager::Get()->IsConnected())
        return;

    switch (key)
    {
    case VK_LEFT:  m_PendingActions.push_back(EInputType::MoveLeft);  break;
    case VK_RIGHT: m_PendingActions.push_back(EInputType::MoveRight); break;
    case VK_UP:    m_PendingActions.push_back(EInputType::ClimbUp);   break;
    case VK_DOWN:  m_PendingActions.push_back(EInputType::ClimbDown); break;
    }
}