#include "stdafx.h"
#include "windows.h"
#include "Render.h"
#include "UILobby.h"
#include "NetworkManager.h"

const int g_nWindowWidth = 1000;
const int g_nWindowHeight = 800;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
    {
        GET_PPK_MGR()->OnKeyDown(wParam);
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        POINT pt{ LOWORD(lParam), HIWORD(lParam) };
        GET_PPK_MGR()->OnClick(pt);
        return 0;
    }
    case WM_DESTROY:
        CNetworkManager::Get()->ShutDown();
        PostQuitMessage(0);
        return 0;
    }
    
    return DefWindowProc(hWnd, message, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, char*, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"PonpokoWindow";

    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"Ponpoko",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, g_nWindowWidth, g_nWindowHeight,
        nullptr, nullptr, hInstance, nullptr);

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    if (!GET_DX_MGR()->Initialize(hwnd, g_nWindowWidth, g_nWindowHeight))
        return 0;

    CNetworkManager::Get()->Initialize();
    GET_PPK_MGR()->EnterLobby();

    LARGE_INTEGER frequency, lastTime, currentTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);

    MSG msg{};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            QueryPerformanceCounter(&currentTime);
            float deltaSeconds = (float)(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
            lastTime = currentTime;

            CPonpokoMgr::Get()->OnUpdate(deltaSeconds);
            CDXManager::Get()->OnUpdate(deltaSeconds);
        }
    }
    return 0;
}