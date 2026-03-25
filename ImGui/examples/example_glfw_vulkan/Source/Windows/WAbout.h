// Copyright @Dave Scholze 2026

#pragma once

#include "imgui.h"

#include "examples/example_glfw_vulkan/Source/LWindow.h"

class WAbout : public LWindow
{
// VIRTUAL FUNCTIONS ===================================================================================================
#pragma region VirtualFunctions
public:

    void Tick(TSettings* settings) override
    {
        ImGui::Begin("About", nullptr, getWindowFlags());

        ImGui::Spacing();
        ImGui::SeparatorText("ABOUT");

        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); // -----------------------------------------------------

        // About the developer. ----------------------------------------------------------------------------------------

        ImGui::Text("ffDAZMpeg is a simple image conversion tool by Dave Scholze aka DaveTheFreak.");
        ImGui::TextLinkOpenURL("DaveTheFreak Website", "https:/www.davethefreak.com");

        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); // -----------------------------------------------------

        // FFMPEG info. ------------------------------------------------------------------------------------------------
        ImGui::Text("For converting the images, the free software ffmpeg is used and required.");
        ImGui::TextLinkOpenURL("ffmpeg Website", "https:/ffmpeg.org/download.html");

        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); // -----------------------------------------------------

        // Dear ImGui info. --------------------------------------------------------------------------------------------
        ImGui::Text("For the GUI, Dear ImGui has been used.");
        ImGui::TextLinkOpenURL("Dear ImGui", "https://github.com/ocornut/imgui");

        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); // -----------------------------------------------------
        ImGui::ShowDemoWindow();

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
