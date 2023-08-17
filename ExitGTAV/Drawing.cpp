#include "Drawing.h"
#include "ExitGTAV.h"
#include "process.h"

#include <string>
#include <iostream>

LPCSTR Drawing::lpWindowName = "ExitGTAV";
ImVec2 Drawing::vWindowSize = { 150, 100 };
ImGuiWindowFlags Drawing::WindowFlags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize;
bool Drawing::bDraw = true;

void Drawing::Active()
{
	bDraw = true;
}

bool Drawing::isActive()
{
	return bDraw == true;
}

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void Drawing::Draw()
{
    if (isActive())
    {
        ImGui::SetNextWindowSize(vWindowSize);
        ImGui::SetNextWindowBgAlpha(1.0f);
        ImGui::Begin(lpWindowName, &bDraw, WindowFlags);
        {
            const wchar_t* processName = L"GTA5.exe";
            bool isGTA5Running = IsProcessRunning(processName);

            if (isGTA5Running) {
                if (ImGui::Button("Exit GTAV")) {
                    ImGui::DestroyContext();
                    main();
                }

                HelpMarker("Get that bag");
            }
            else {
                ImGui::Text("GTAV is not running.");
                ImGui::Text("Waiting...");
            }
        }
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f));
        ImGui::PopStyleVar(1);
        ImGui::PopStyleColor(1);
    }
}