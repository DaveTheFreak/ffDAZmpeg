// Copyright @Dave Scholze 2026

#pragma once

#include "imgui.h"

#include "examples/example_glfw_vulkan/Source/LWindow.h"

class WSettings : public LWindow
{
// VIRTUAL FUNCTIONS ===================================================================================================
#pragma region VirtualFunctions
public:

    void Tick(TSettings* settings) override
    {
        ImGui::Begin("Settings", nullptr, getWindowFlags());

        ImGui::Spacing();
        ImGui::SeparatorText("SETTINGS");

        // -------------------------------------------------------------------------------------------------------------
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

        // ffmpeg location ---------------------------------------------------------------------------------------------
        ImGui::InputText("ffmpeg location", settings->savedValues.ffmpegExecutablePath, IM_ARRAYSIZE(settings->savedValues.ffmpegExecutablePath));
            ImGui::SetItemTooltip("File path to the executable, this program won't run without it.");

        ImGui::Spacing();

        ImGui::Checkbox("Enable ffmpeg Log", &settings->savedValues.generateFfmpegLog);
            ImGui::SetItemTooltip("If enabled, will output the ffmpeg log to the output folder.");

        // -------------------------------------------------------------------------------------------------------------
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

        if (ImGui::TreeNode("More Info"))
        {
            ImGui::TextLinkOpenURL("Download ffmpeg", "https:/ffmpeg.org/download.html");
            ImGui::Spacing();
            ImGui::Text("Unpack somewhere, open the folder and locate the \"bin\" folder.");
            ImGui::Text("Copy past the path above.");

            ImGui::TreePop();
        }

        // -------------------------------------------------------------------------------------------------------------
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

        // Save settings -----------------------------------------------------------------------------------------------
        if (ImGui::Button("Revert Settings"))
        {
            ImGui::OpenPopup("Revert Settings?");
        }

            if (ImGui::BeginPopupModal("Revert Settings?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Are you sure?");
                ImGui::Separator();

                if (ImGui::Button("Yes", ImVec2(120, 0)))
                {
                    settings->saveSettings();
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();

                if (ImGui::Button("No", ImVec2(120, 0)))
                {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

        ImGui::SameLine();

        if (ImGui::Button("Save Settings"))
        {
            settings->saveSettings();
        }   ImGui::SetItemTooltip("Save current settings->");

        ImGui::End();
    }

#pragma endregion

// METHODS =============================================================================================================
#pragma region Methods

    // ...

#pragma endregion

// VARIABLES ===========================================================================================================
#pragma region Variables

    // ...

#pragma endregion

};
