#include "stdafx.h"
#include "UIMapEditor.h"
#include <filesystem>
#include <cstdlib>
#include <ctime>
#include <unordered_map>

#include <commdlg.h>
#pragma comment(lib, "comdlg32.lib")

using namespace Ponpoko;

std::unordered_map<EEditorBrush, std::wstring> LabelStringMap
{
    {EEditorBrush::PLATFORM, L"Platform"},
    {EEditorBrush::LADDER, L"Ladder"},
    {EEditorBrush::SPIKE, L"Spike"},
    {EEditorBrush::ITEM, L"Item"},
    {EEditorBrush::ENEMY, L"Enemy"},
    {EEditorBrush::START_POS, L"Start"},
};

CUIMapEditor::CUIMapEditor()
{
    srand((unsigned)time(nullptr));
    m_pMapTexture = GET_DX_MGR()->CreateSolidTexture(10, 10, D3DCOLOR_ARGB(255, 255, 255, 0));
    m_pButtonTexture = GET_DX_MGR()->CreateSolidTexture(m_OptionButtonWidth, m_OptionButtonHeight, D3DCOLOR_ARGB(255, 40, 40, 60));
    m_pButtonSelectedTexture = GET_DX_MGR()->CreateSolidTexture(m_OptionButtonWidth, m_OptionButtonHeight, D3DCOLOR_ARGB(255, 90, 90, 40));

    RECT clientRect;
    GetClientRect(GET_DX_MGR()->GetHWND(), &clientRect);
    int nWidth = clientRect.right - clientRect.left;

    for (int i = 0; i < 6; ++i)
    {
        int nLeft = 10 + i * (m_OptionButtonWidth + m_OptionButtonGap);
        m_OptionRects[i] = RECT{ nLeft, 10, nLeft + m_OptionButtonWidth, 10 + m_OptionButtonHeight };

    }
}

CUIMapEditor::~CUIMapEditor()
{
    if (m_pMapTexture) m_pMapTexture->Release();
    if (m_pButtonTexture) m_pButtonTexture->Release();
    if (m_pButtonSelectedTexture) m_pButtonSelectedTexture->Release();
}

void CUIMapEditor::OnRender()
{
    DrawMapData();
    DrawOptionButtons();
}

void CUIMapEditor::DrawMapData()
{
    m_MapData.DrawMapData(m_pMapTexture);
}

void CUIMapEditor::DrawOptionButtons()
{
    for (int i = 0; i < 6; ++i)
    {
        EEditorBrush eBrush = (EEditorBrush)i;
        IDirect3DTexture9* pTex = (eBrush == m_eCurrentBrush) ? m_pButtonSelectedTexture : m_pButtonTexture;

        GET_DX_MGR()->DrawTexture(pTex, (float)m_OptionRects[i].left, (float)m_OptionRects[i].top);
        GET_DX_MGR()->DrawTextBlock(LabelStringMap[eBrush].c_str(), m_OptionRects[i], D3DCOLOR_ARGB(255, 255, 255, 255));
    }
}

bool CUIMapEditor::HasPlatform(float x, float y) const
{
    if (y == Ponpoko::SMapData::m_fFloorY)
        return true;

    for (const auto& platform : m_MapData.Platforms)
    {
        if (platform.fY == y && x >= platform.fStartX && x <= platform.fEndX)
            return true;
    }
    return false;
}

bool CUIMapEditor::HasLadder(int nFloor) const
{
    for (const auto& ladder : m_MapData.Ladders)
    {
        if (ladder.nFloor == nFloor)
            return true;
    }
    return false;
}

bool CUIMapEditor::TryClickOptionButton(POINT InPT)
{
    for (int i = 0; i < 6; ++i)
    {
        if (PtInRect(&m_OptionRects[i], InPT))
        {
            m_eCurrentBrush = (EEditorBrush)i;
            m_bHasPendingPoint = false;
            return true;
        }
    }
    return false;
}

void CUIMapEditor::OnClick(POINT InPT)
{
    if (TryClickOptionButton(InPT))
        return;

    PlaceAtPoint(InPT);
}

void CUIMapEditor::PlaceAtPoint(POINT InPT)
{
    SPos pos{ (float)InPT.x, (float)InPT.y };

    switch (m_eCurrentBrush)
    {
    case EEditorBrush::PLATFORM:
        if (!m_bHasPendingPoint) { m_PendingPoint = pos; m_bHasPendingPoint = true; }
        else
        {
            SPlatform platform;
            platform.fY = SnapToRow(m_PendingPoint.y);
            platform.fStartX = min(m_PendingPoint.x, pos.x);
            platform.fEndX = max(m_PendingPoint.x, pos.x);
            m_MapData.Platforms.push_back(platform);
            m_bHasPendingPoint = false;
        }
        break;
    case EEditorBrush::LADDER:
    {
        int nTopRow = SnapToSegment(pos.y);
        int nBottomRow = nTopRow + 1;

        if (HasPlatform(pos.x, m_MapData.GetRowY(nTopRow))
            && HasPlatform(pos.x, m_MapData.GetRowY(nBottomRow))
            && !HasLadder(nTopRow))
        {
            m_MapData.Ladders.push_back({ pos.x, nTopRow });
        }
    }
    break;

    case EEditorBrush::SPIKE:
        m_MapData.Spikes.push_back({ pos });
        break;

    case EEditorBrush::ITEM:
    {
        float fRowY = (float)SnapToRow(pos.y);
        if (HasPlatform(pos.x, fRowY))
        {
            EItemType eType = (rand() % 2 == 0) ? EItemType::CARROT : EItemType::CHERRY;
            int nScore = (eType == EItemType::CARROT) ? 10 : 20;
            m_MapData.Items.push_back({ SPos{ pos.x, fRowY }, eType, nScore });
        }
    }
    break;

    case EEditorBrush::ENEMY:
        m_MapData.Enemies.push_back({ pos });
        break;

    case EEditorBrush::START_POS:
        m_MapData.StartPos = SPos{ pos.x, SMapData::m_fFloorY };
        break;
    }
}

int CUIMapEditor::SnapToRow(float y) const
{
    int nClosestIndex = 0;
    float fMinDist = fabsf(y - m_MapData.GetRowY(0));
    int nPlatformRowCount = m_MapData.GetRowCount();

    for (int i = 1; i < nPlatformRowCount; ++i)
    {
        float fDist = fabsf(y - m_MapData.GetRowY(i));
        if (fDist < fMinDist)
        {
            fMinDist = fDist;
            nClosestIndex = i;
        }
    }

    return m_MapData.GetRowY(nClosestIndex);
}

int CUIMapEditor::SnapToSegment(float y) const
{
    int nSegmentCount = m_MapData.GetRowCount();
    int nClosestSegment = 0;
    float fMinDist = fabsf(y - (m_MapData.GetRowY(0) + m_MapData.GetRowY(1)) * 0.5f);

    for (int i = 1; i < nSegmentCount; ++i)
    {
        float fMidY = (m_MapData.GetRowY(i) + m_MapData.GetRowY(i + 1)) * 0.5f;
        float fDist = fabsf(y - fMidY);
        if (fDist < fMinDist)
        {
            fMinDist = fDist;
            nClosestSegment = i;
        }
    }

    return nClosestSegment;
}

void CUIMapEditor::SaveMapDataWithDialog()
{
    wchar_t szFile[MAX_PATH] = L"stage.map";
    std::string curDir = std::filesystem::current_path().string();
    std::wstring wstrcurDir(curDir.begin(), curDir.end());

    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GET_DX_MGR()->GetHWND();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrInitialDir = wstrcurDir.c_str();
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

    if (GetSaveFileNameW(&ofn))
        m_MapData.SaveMapData(szFile);
}

void CUIMapEditor::OnKeyDown(WPARAM key)
{
    if (key != 'S')
        return;

    bool bCtrlHeld = (GetKeyState(VK_CONTROL) & 0x8000) != 0;

    if (bCtrlHeld)
        SaveMapDataWithDialog();
    else
        m_MapData.SaveMapData(L"stage.map");
}
