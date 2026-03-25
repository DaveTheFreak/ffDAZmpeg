// Copyright @Dave Scholze 2026

#pragma once
#include "imgui.h"
#include "Settings/TSettings.h"

class LWindow
{
public:

    virtual ~LWindow() = default;

    virtual void OnBeginPlay(TSettings* settings){}

    virtual void Tick(TSettings* settings){}

    virtual void OnEndPlay(TSettings* settings){}

    /** Get window flags to create a new fullscreen window. */
    static ImGuiWindowFlags getWindowFlags()
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
                                        ImGuiWindowFlags_NoMove |
                                        ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoSavedSettings;
        return window_flags;
    }

};
