#pragma once

#include <string> 
#include <stdio.h>
#include <dwmapi.h>

#include <d3d11.h>
#include "Thirdparty/Microsoft DirectX SDK/Include/D3D11.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "Thirdparty/Dear ImGui/backends/imgui_impl_dx11.h"
#include "Thirdparty/Dear ImGui/backends/imgui_impl_win32.h"
#include "Thirdparty/Dear ImGui/imgui.h"
#include "Thirdparty/Dear ImGui/imgui_internal.h"

#include "DirectX.h"
#include "Window.h"
#include "Gui.h"
#include "Globals.h"