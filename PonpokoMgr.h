#pragma once
#include "Singleton.h"
#include <memory>
#include <windows.h>
#include <string>

enum class EGameState
{
    LOBBY,
    SINGLE_GAME,
    MULTI_GAME,
    REPLAY,
    MAP_EDITOR,
    ENUM_END
};

class CUILobby;
class CUIMapEditor;
class CUISingleGame;
class CUIMultiGame;

class CPonpokoMgr : public ISingleton<CPonpokoMgr>
{
public:
    CPonpokoMgr() {}
    virtual ~CPonpokoMgr() {}

public:
    void OnUpdate(float fDelta);
    void OnClick(POINT InCoord);
    void OnKeyDown(WPARAM Inkey);

    void EnterLobby();
    void EnterSingleGame();
    void EnterMultiGame(bool bIsHost, const std::string& InJoinIP = "127.0.0.1");
    void EnterReplay();
    void EnterMapEditor();

private:
    EGameState m_eCurrentState = EGameState::LOBBY;
    std::shared_ptr<CUILobby> m_pLobby;
    std::shared_ptr<CUIMapEditor> m_pMapEditor;
    std::shared_ptr<CUISingleGame> m_pSingleGame;
    std::shared_ptr<CUIMultiGame> m_pMultiGame;
};
