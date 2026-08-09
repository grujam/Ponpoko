#pragma once
#include "Render.h"
#include "PonpokoHelper.h"
#include "PlayerSession.h"

class CUISingleGame : public IRenderable
{
public:
    CUISingleGame();
    virtual ~CUISingleGame();

public:
    virtual void OnRender() override;
    void OnUpdate(float fDelta);
    void OnKeyDown(WPARAM key);

private:
    void StartLoadingLevel(int nLevel);
    void DrawLoadingScreen();
    void DrawPlayer();

private:
    Ponpoko::SMapData m_MapData;
    IDirect3DTexture9* m_pMapTexture = nullptr;
    IDirect3DTexture9* m_pPlayerTexture = nullptr;

    CPlayerSession m_Player;

    int m_nCurrentLevel = 1;
    bool m_bLoading = true;
    float m_fLoadingTimer = 0.0f;

    constexpr static float m_fLoadingDuration = 1.0f;
};