#include "stdafx.h"
#include "UILobby.h"

CUILobby::CUILobby()
{
    m_pLobbyTexture = GET_DX_MGR()->LoadTexture(L"lobby.png");
    m_pButtonTexture = GET_DX_MGR()->CreateSolidTexture(m_ButtonWidth, m_ButtonHeight, D3DCOLOR_ARGB(255, 40, 40, 60));
}

CUILobby::~CUILobby()
{
    if (m_pLobbyTexture)
        m_pLobbyTexture->Release();
    if (m_pButtonTexture)
        m_pButtonTexture->Release();
}

void CUILobby::OnRender()
{
    DrawBackground();

    RECT rect;
    GetClientRect(GET_DX_MGR()->GetHWND(), &rect);

    int nCenterX = (rect.right - rect.left) / 2;
    int nCenterY = (rect.bottom - rect.top) / 2;

    int nTotalHeight = m_ButtonHeight * 5 + m_ButtonGap * 4;
    int nStartY = nCenterY - nTotalHeight / 2;

    m_SingleButtonRect = RECT{ nCenterX - m_ButtonWidth / 2, nStartY, nCenterX + m_ButtonWidth / 2, nStartY + m_ButtonHeight };
    m_MultiButtonRect = RECT{ nCenterX - m_ButtonWidth / 2, m_SingleButtonRect.bottom + m_ButtonGap, nCenterX + m_ButtonWidth / 2, m_SingleButtonRect.bottom + m_ButtonGap + m_ButtonHeight };
    m_ReplayButtonRect = RECT{ nCenterX - m_ButtonWidth / 2, m_MultiButtonRect.bottom + m_ButtonGap, nCenterX + m_ButtonWidth / 2, m_MultiButtonRect.bottom + m_ButtonGap + m_ButtonHeight };
    m_MapEditorButtonRect = RECT{ nCenterX - m_ButtonWidth / 2, m_ReplayButtonRect.bottom + m_ButtonGap, nCenterX + m_ButtonWidth / 2, m_ReplayButtonRect.bottom + m_ButtonGap + m_ButtonHeight };
    m_ExitButtonRect = RECT{ nCenterX - m_ButtonWidth / 2, m_MapEditorButtonRect.bottom + m_ButtonGap, nCenterX + m_ButtonWidth / 2, m_MapEditorButtonRect.bottom + m_ButtonGap + m_ButtonHeight };

    DrawButton(m_SingleButtonRect, L"싱글 게임");
    DrawButton(m_MultiButtonRect, L"멀티 게임");
    DrawButton(m_ReplayButtonRect, L"리플레이");
    DrawButton(m_MapEditorButtonRect, L"맵 에디터");
    DrawButton(m_ExitButtonRect, L"게임 종료");
}

void CUILobby::DrawBackground()
{
    GET_DX_MGR()->DrawTexture(m_pLobbyTexture, 0, 0, 255);
}

void CUILobby::DrawButton(const RECT& InRect, const wchar_t* InLabel)
{
    GET_DX_MGR()->DrawTexture(m_pButtonTexture, (float)InRect.left, (float)InRect.top);
    GET_DX_MGR()->DrawTextBlock(InLabel, InRect, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CUILobby::OnClick(POINT InPT)
{
    if (PtInRect(&m_SingleButtonRect, InPT))
        GET_PPK_MGR()->EnterSingleGame();
    else if (PtInRect(&m_MultiButtonRect, InPT))
    {
        int nResult = MessageBoxW(GET_DX_MGR()->GetHWND(), L"호스트로 시작하시겠습니까?", L"Ponpoko", MB_YESNO);

        if (nResult == IDYES)
            GET_PPK_MGR()->EnterMultiGame(true);
        else
            GET_PPK_MGR()->EnterMultiGame(false, "127.0.0.1");
    }
    else if (PtInRect(&m_ReplayButtonRect, InPT))
        GET_PPK_MGR()->EnterReplay();
    else if (PtInRect(&m_MapEditorButtonRect, InPT))
        GET_PPK_MGR()->EnterMapEditor();
    else if (PtInRect(&m_ExitButtonRect, InPT))
        PostQuitMessage(0);
}