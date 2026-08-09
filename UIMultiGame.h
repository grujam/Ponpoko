#pragma once
#include "Render.h"
#include "PonpokoHelper.h"
#include "NetworkManager.h"
#include "PlayerSession.h"
#include <vector>
#include <string>

class CUIMultiGame : public IRenderable
{
public:
    CUIMultiGame(bool bIsHost, const std::string& InJoinIP);
    virtual ~CUIMultiGame();

public:
    virtual void OnRender() override;
    void OnUpdate(float fDelta);
    void OnKeyDown(WPARAM key);

private:
    void StartLoadingLevel(int nLevel);
    void DrawLoadingScreen(const wchar_t* InText);
    void LockStepTick();
    void DrawPlayer(const CPlayerSession& InPlayer, IDirect3DTexture9* pTexture);

private:
    bool m_bIsHost = false;

    Ponpoko::SMapData m_MapData;
    IDirect3DTexture9* m_pMapTexture = nullptr;
    IDirect3DTexture9* m_pLocalPlayerTexture = nullptr;
    IDirect3DTexture9* m_pRemotePlayerTexture = nullptr;

    CPlayerSession m_LocalSession;
    CPlayerSession m_RemoteSession;

    int m_nCurrentLevel = 1;
    bool m_bLoadingLevel = true;
    float m_fLoadingTimer = 0.0f;

    std::vector<EInputType> m_PendingActions;
    bool m_bTickSent = false;
    SPonpokoPacket m_LastLocalPacket;
    int m_nCurrentTick = 0;
    float m_fTickAccumulator = 0.0f;

    constexpr static unsigned short m_nPort = 5001;
    constexpr static float m_fLoadingDuration = 1.0f;
    constexpr static float m_fTickDuration = 1.0f / 60.0f;
};