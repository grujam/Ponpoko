#pragma once
#include "Render.h"
#include "PonpokoHelper.h"

enum class EEditorBrush
{
    PLATFORM,
    LADDER,
    SPIKE,
    ITEM,
    ENEMY,
    START_POS,
};

class CUIMapEditor : public IRenderable
{
public:
    CUIMapEditor();
    virtual ~CUIMapEditor();

public:
    virtual void OnRender() override;
    void OnClick(POINT InPT);
    void OnKeyDown(WPARAM key);

private:
    void DrawMapData();
    void DrawOptionButtons();
    bool HasPlatform(float x, float y) const;
    bool HasLadder(int nFloor) const;
    bool TryClickOptionButton(POINT InPT);
    void PlaceAtPoint(POINT InPT);

    int SnapToRow(float y) const;
    int SnapToSegment(float y) const;
    
    void SaveMapDataWithDialog();

private:
    Ponpoko::SMapData m_MapData;

    EEditorBrush m_eCurrentBrush = EEditorBrush::PLATFORM;
    bool m_bHasPendingPoint = false;
    Ponpoko::SPos m_PendingPoint;

    IDirect3DTexture9* m_pMapTexture = nullptr;
    IDirect3DTexture9* m_pButtonTexture = nullptr;
    IDirect3DTexture9* m_pButtonSelectedTexture = nullptr;

    RECT m_OptionRects[6]{};

private:
    constexpr static int m_OptionButtonWidth = 140;
    constexpr static int m_OptionButtonHeight = 40;
    constexpr static int m_OptionButtonGap = 5;
};