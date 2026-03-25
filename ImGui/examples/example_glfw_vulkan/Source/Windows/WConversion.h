// Copyright @Dave Scholze 2026

#pragma once

#include "imgui.h"

#include "examples/example_glfw_vulkan/Source/LWindow.h"
#include "examples/example_glfw_vulkan/Source/Types/Enums/EEncodingBitDepth.h"

class WConversion : public LWindow
{
public:

    WConversion()
    {
        ffmpegExecutionPtr = new ffmpeg_execution();
    }

    ~WConversion() override
    {
        delete ffmpegExecutionPtr;
        ffmpegExecutionPtr = nullptr;
    }

private:
    // VIRTUAL FUNCTIONS ===================================================================================================
#pragma region VirtualFunctions
public:

    void Tick(TSettings* settings) override
    {
        ImGui::Begin("Conversion", nullptr, getWindowFlags());

        ImGui::Spacing();
        ImGui::SeparatorText("CONVERSION");

        // -------------------------------------------------------------------------------------------------------------
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

        // Start the tree node. ----------------------------------------------------------------------------------------

        // FILE PATHS --------------------------------------------------------------------------------------------------
        if (ImGui::CollapsingHeader("FILE PATHS"))
        {
            ImGui::Indent();

            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); // -------------------------------------------------

            // Paths - Explanation -------------------------------------------------------------------------------------
            ImGui::Text(R"(Images must be in ".png" or ".tif" format.)");

            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); // -------------------------------------------------

            // Actual inputs -------------------------------------------------------------------------------------------
            ImGui::InputText(
                "Daz Image(s) Input Path",
                settings->savedValues.inputPath,
                IM_ARRAYSIZE(settings->savedValues.inputPath));
            ImGui::SetItemTooltip(R"(Folder where the ".png" or ".tif" files from Daz are.)");

            ImGui::Spacing(); // ---------------------------------------------------------------------------------------

            ImGui::InputText(
                "Daz Image(s) Output Path",
                settings->savedValues.outputPath,
                IM_ARRAYSIZE(settings->savedValues.outputPath));
            ImGui::SetItemTooltip("Save folder for converted files.");

            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); // -------------------------------------------------
            ImGui::Unindent();
        }

        // EFFECTS -----------------------------------------------------------------------------------------------------
        if (ImGui::CollapsingHeader("EFFECTS"))
        {
            ImGui::Indent();
            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); // -------------------------------------------------

            // Chromatic aberration. -----------------------------------------------------------------------------------
            ImGui::SliderFloat(
                "Chromatic Aberration",
                &settings->savedValues.chromaticAberrationIntensity,
                0.0f, 0.01f, "%.4f");
            ImGui::SetItemTooltip(
                "Adds chromatic aberration at the borders to simulate a real lens.");

            ImGui::Spacing(); // ---------------------------------------------------------------------------------------

            // Image enhancement. --------------------------------------------------------------------------------------
            ImGui::Checkbox("Enable Image Enhancement", &settings->savedValues.enableImageEnhancement);
            ImGui::SetItemTooltip(
                "Reduces banding and adds micro detail for a better compression,\nbut loses minimal amount of detail.");

            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); // -------------------------------------------------

            // HDR curve. ----------------------------------------------------------------------------------------------
            if (ImGui::CollapsingHeader("HDR Curve"))
            {
                ImGui::Indent();

                static constexpr ImVec2 HDR_CURVE_VEC2 = {80.f, 300.f};
                static constexpr float HDR_CURVE_MIN = -0.25f;
                static constexpr float HDR_CURVE_MAX = 0.25f;

                ImGui::VSliderFloat("0.0", HDR_CURVE_VEC2, &settings->savedValues.hdrCurve[0], HDR_CURVE_MIN, HDR_CURVE_MAX);
                ImGui::SameLine(0, 20.f);

                ImGui::VSliderFloat("0.1", HDR_CURVE_VEC2, &settings->savedValues.hdrCurve[1], HDR_CURVE_MIN, HDR_CURVE_MAX);
                ImGui::SameLine(0, 20.f);

                ImGui::VSliderFloat("0.5", HDR_CURVE_VEC2, &settings->savedValues.hdrCurve[2], HDR_CURVE_MIN, HDR_CURVE_MAX);
                ImGui::SameLine(0, 20.f);

                ImGui::VSliderFloat("0.8", HDR_CURVE_VEC2, &settings->savedValues.hdrCurve[3], HDR_CURVE_MIN, HDR_CURVE_MAX);
                ImGui::SameLine(0, 20.f);

                ImGui::VSliderFloat("1.0", HDR_CURVE_VEC2, &settings->savedValues.hdrCurve[4], HDR_CURVE_MIN, HDR_CURVE_MAX);

                settings->savedValues.hdrCurve[0] = std::clamp(settings->savedValues.hdrCurve[0], 0.0f, 1.0f);
                settings->savedValues.hdrCurve[1] = std::clamp(settings->savedValues.hdrCurve[1], -0.1f, 0.9f);
                settings->savedValues.hdrCurve[2] = std::clamp(settings->savedValues.hdrCurve[2], -0.5f, 0.5f);
                settings->savedValues.hdrCurve[3] = std::clamp(settings->savedValues.hdrCurve[3], -0.8f, 0.2f);
                settings->savedValues.hdrCurve[4] = std::clamp(settings->savedValues.hdrCurve[4], -1.0f, 0.0f);

                ImGui::Unindent();
            }

            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); // -------------------------------------------------

            // Logo file path. -----------------------------------------------------------------------------------------
            ImGui::InputText(
                "Logo File Path",
                settings->savedValues.logoPath, IM_ARRAYSIZE(settings->savedValues.logoPath));
            ImGui::SetItemTooltip(
                "Leave empty for no logo.\nLogo must be \".png\" with alpha-channel.\nFile must have same aspect ratio as final image.\nLogo must be scaled and placed correctly.");

            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); // -------------------------------------------------
            ImGui::Unindent();
        }

        // ENCODING ----------------------------------------------------------------------------------------------------
        if (ImGui::CollapsingHeader("ENCODING"))
        {
            ImGui::Indent();
            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); // -------------------------------------------------

                // Format ----------------------------------------------------------------------------------------------
                ffd::drop_down::draw_enum<EEncodingImageFormats>(settings->savedValues.selectedImageFormat);
                ImGui::SetItemTooltip("Only avif is currently working.");

                ImGui::Spacing();

                // Dynamic Range ---------------------------------------------------------------------------------------
                ffd::drop_down::draw_enum<EEncodingDynamicRangeModes>(settings->savedValues.selectedDynamicRangeMode);
                ImGui::SetItemTooltip("HDR requires capable image viewers that support HDR PQ.");

                ImGui::Spacing();

                // Bit depth -------------------------------------------------------------------------------------------
                ffd::drop_down::draw_enum<EEncodingBitDepth>(settings->savedValues.selectedBitDepth);
                ImGui::SetItemTooltip("Note: JPEG does not support 10 bits.");

                    // HDR = always 10 bit
                    if (settings->savedValues.selectedDynamicRangeMode == EEncodingDynamicRangeModes::Type::HDR)
                        settings->savedValues.selectedBitDepth = EEncodingBitDepth::Type::x10;
                    // JPG = always 8 bit and SDR
                    if (settings->savedValues.selectedImageFormat == EEncodingImageFormats::Type::jpg)
                    {
                        settings->savedValues.selectedBitDepth = EEncodingBitDepth::Type::x10;
                        settings->savedValues.selectedDynamicRangeMode = EEncodingDynamicRangeModes::Type::SDR;
                    }

            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); // -------------------------------------------------

                // Encoder ---------------------------------------------------------------------------------------------

                ffd::drop_down::draw_enum<EEncodingAcceleration>(settings->savedValues.selectEncodingAcceleration);
                ImGui::SetItemTooltip("Auto will select vendor specific options automatically.");

            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); // -------------------------------------------------
            ImGui::Unindent();
        }

        // CONVERT -----------------------------------------------------------------------------------------------------
            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); // -------------------------------------------------

            if (ImGui::Button("START CONVERSION"))
            {
                ffmpegExecutionPtr->Run(settings);
            }   ImGui::SetItemTooltip("Start converting the images.");

            // Progress Bar --------------------------------------------------------------------------------------------
            ImGui::SameLine();
            ImGui::ProgressBar(settings->runtimeValues.conversionProgress);

            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); // -------------------------------------------------

        ImGui::End();
    }

#pragma endregion

// METHODS =============================================================================================================
#pragma region Methods

    // ...

#pragma endregion

// VARIABLES ===========================================================================================================
#pragma region Variables

    ffmpeg_execution* ffmpegExecutionPtr {nullptr};

#pragma endregion

};
