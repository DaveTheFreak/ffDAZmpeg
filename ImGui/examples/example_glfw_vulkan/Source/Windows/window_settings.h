#pragma once

#include "imgui.h"

struct window_settings
{
    void Draw()
    {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
                                ImGuiWindowFlags_NoMove |
                                ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoSavedSettings;

        ImGui::Begin("Settings", nullptr, window_flags);

        ImGui::Text("New class, yeah.");

        ImGui::End();
    }
};
