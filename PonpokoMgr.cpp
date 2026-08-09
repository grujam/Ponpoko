#include "stdafx.h"
#include "Render.h"
#include "UILobby.h"
#include "PonpokoHelper.h"
#include "UIMapEditor.h"
#include "UISingleGame.h"
#include "UIMultiGame.h"
#include <fstream>

void CPonpokoMgr::OnUpdate(float fDelta)
{
    switch (m_eCurrentState)
    {
    case EGameState::SINGLE_GAME:
        m_pSingleGame->OnUpdate(fDelta);
        break;
    case EGameState::MULTI_GAME:
        m_pMultiGame->OnUpdate(fDelta);
        break;
    }
}

void CPonpokoMgr::OnClick(POINT InCoord)
{
    switch (m_eCurrentState)
    {
    case EGameState::LOBBY:
        m_pLobby->OnClick(InCoord);
        break;
    case EGameState::MAP_EDITOR:
        m_pMapEditor->OnClick(InCoord);
        break;
    }
}

void CPonpokoMgr::OnKeyDown(WPARAM Inkey)
{
    switch (m_eCurrentState)
    {
    case EGameState::MAP_EDITOR:
        m_pMapEditor->OnKeyDown(Inkey);
        break;
    case EGameState::SINGLE_GAME:
        m_pSingleGame->OnKeyDown(Inkey);
        break;
    case EGameState::MULTI_GAME:
        m_pMultiGame->OnKeyDown(Inkey);
        break;
    }
}

void CPonpokoMgr::EnterLobby()
{
    if (m_pSingleGame)
    {
        Render::EraseRenderable(m_pSingleGame);
        m_pSingleGame.reset();
    }
    if (m_pMultiGame)
    {
        Render::EraseRenderable(m_pMultiGame);
        m_pMultiGame.reset();
        CNetworkManager::Get()->Disconnect();
    }

    m_eCurrentState = EGameState::LOBBY;
    m_pLobby = Render::CreateRenderable<CUILobby>();
}

void CPonpokoMgr::EnterSingleGame()
{
    Render::EraseRenderable(m_pLobby);
    m_eCurrentState = EGameState::SINGLE_GAME;
    m_pSingleGame = Render::CreateRenderable<CUISingleGame>();
}

void CPonpokoMgr::EnterMultiGame(bool bIsHost, const std::string& InJoinIP)
{
    Render::EraseRenderable(m_pLobby);
    m_eCurrentState = EGameState::MULTI_GAME;
    m_pMultiGame = Render::CreateRenderable<CUIMultiGame>(bIsHost, InJoinIP);
}

void CPonpokoMgr::EnterReplay()
{
    Render::EraseRenderable(m_pLobby);
    m_eCurrentState = EGameState::REPLAY;
}

void CPonpokoMgr::EnterMapEditor()
{
    Render::EraseRenderable(m_pLobby);
    m_eCurrentState = EGameState::MAP_EDITOR;
    m_pMapEditor = Render::CreateRenderable<CUIMapEditor>();
}