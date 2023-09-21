#include "StdAfx.h"

#include "DirectX.h"
#include "Window.h"
#include "Gui.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Window::CreateWindowAndClass();
    DirectX::Init();
    Window::Setup();
    Gui::Init(Window::hWnd, DirectX::m_pDevice, DirectX::m_pDeviceContext); // Mive into DirectX or use g_Globals
    
    while (Window::bIsRunning)
    {
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                Window::bIsRunning = false;
        }

        Gui::Render();
        DirectX::Present();
    }

    Gui::Unload();
    DirectX::CleanupDeviceD3D();
    Window::Shutdown();

    return 0;
}
