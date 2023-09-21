#include "StdAfx.h"
#include "Gui.h"
#include "ExitGTAV.h"
#include "process.h"

void Gui::Init( HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* device_context )
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL; // Remove the imgui.ini
    GImGui->NavDisableHighlight = true;

    ImGui::GetDefaultFont();
    Customize();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(device, device_context);
}

void Gui::Customize( void )
{
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding    = 4;
    style.ChildRounding     = 3;
    style.FrameRounding     = 3;
    style.PopupRounding     = 3;
    style.GrabRounding      = 3;
    style.TabRounding       = 3;
    style.ScrollbarRounding = 3;
    style.ButtonTextAlign   = { 0.5f, 0.5f };
    style.WindowTitleAlign  = { 0.5f, 0.5f };
    style.ItemSpacing       = { 8.0f, 8.0f };
    style.WindowPadding     = { 8.0f, 8.0f };
    style.FramePadding      = { 4.0f, 4.0f };
    style.WindowBorderSize  = 1;
    style.FrameBorderSize   = 1;
    style.ScrollbarSize     = 12.f;
    style.GrabMinSize       = 8.f;
    style.WindowShadowSize  = 0.f;

    style.Colors[ImGuiCol_WindowBg]         = ImVec4(0.06f, 0.06f, 0.06f, 0.70f);
    style.Colors[ImGuiCol_WindowShadow]     = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
}

void Gui::Think( void )
{
    IsWindowCollapsed = ImGui::IsWindowCollapsed();

    // Handle Collapse
    if (IsWindowCollapsed)
    {
        SetWindowPos(Window::hWnd, NULL, 0, 0, Window::WindowSize.x, Gui::TitleBarHeight, SWP_NOMOVE | SWP_NOZORDER);
    }
    else
    {
        SetWindowPos(Window::hWnd, NULL, 0, 0, Window::WindowSize.x, Window::WindowSize.y, SWP_NOMOVE | SWP_NOZORDER);
        Window::WindowSize = { static_cast<short>(ImGui::GetWindowWidth()), static_cast<short>(ImGui::GetWindowHeight()) };
    }
}

void Gui::Render( void )
{
    // Tools
    ImGuiStyle& style = ImGui::GetStyle();

    // Window Infos
    TitleBarHeight = ImGui::GetFrameHeight();
    CloseButtonWidth = ImGui::GetFontSize() + style.FramePadding.x * 2 + style.WindowBorderSize * 2;

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    {
        ImGuiWindowFlags main_window_flags = 0; // ImGuiWindowFlags_NoResize
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(Window::WindowSize.x, Window::WindowSize.y));
        ImGui::Begin(GUI_TITE, &Window::bIsRunning, main_window_flags);
        Think();
        {
            const wchar_t* processName = L"GTA5.exe";
            bool isGTA5Running = IsProcessRunning(processName);

            if (isGTA5Running) {
                if (ImGui::Button("Exit GTAV") || (ImGui::IsKeyPressed(ImGuiKey_P) &&
                    ImGui::IsKeyDown(ImGuiKey_LeftCtrl) &&
                    ImGui::IsKeyDown(ImGuiKey_LeftShift))) {
                    main();
                }

                ImGui::Text("Shortcut: Ctrl + Shift + P");
            }
            else {
                ImGui::Text("GTAV is not running.");
                ImGui::Text("Waiting...");
            }

            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Checkbox("Enable V-Sync", &DirectX::bSyncInterval);
        }
        ImGui::End();
    }
    // Rendering
    ImGui::Render();
}

void Gui::Unload( void )
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
