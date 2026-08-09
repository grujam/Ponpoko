#pragma once
#include "Render.h"

class CUILobby : public IRenderable
{
public:
    CUILobby();
    virtual ~CUILobby();

public:
    virtual void OnRender() override;
    void OnClick(POINT InPT);

private:
    void DrawBackground();
    void DrawButton(const RECT& InRect, const wchar_t* InLabel);

private:
    RECT m_SingleButtonRect{};
    RECT m_MultiButtonRect{};
    RECT m_ReplayButtonRect{};
    RECT m_MapEditorButtonRect{};
    RECT m_ExitButtonRect{};

    IDirect3DTexture9* m_pLobbyTexture = nullptr;
    IDirect3DTexture9* m_pButtonTexture = nullptr;

private:
    constexpr static int m_ButtonWidth = 300;
    constexpr static int m_ButtonHeight = 60;
    constexpr static int m_ButtonGap = 30;
};