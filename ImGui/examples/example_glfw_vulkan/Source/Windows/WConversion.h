// Copyright @Dave Scholze 2026

#pragma once

#include "imgui.h"

#include "examples/example_glfw_vulkan/Source/LWindow.h"

class WConversion : public LWindow
{
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
                static const char* imageFormatsAvailable[] = { "AVIF", "JPEG" };
                uint8_t currentFormatIndex = static_cast<uint8_t>(settings->savedValues.selectedImageFormat);

                if (ImGui::BeginCombo("Format", imageFormatsAvailable[currentFormatIndex]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(imageFormatsAvailable); n++)
                    {
                        bool isSelected = (currentFormatIndex == n);

                        if (ImGui::Selectable(imageFormatsAvailable[n], isSelected))
                            settings->savedValues.selectedImageFormat = static_cast<EEncodingImageFormats>(n);

                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::SetItemTooltip("Currently only AVIF is being supported.");

                ImGui::Spacing();

                // Dynamic Range ---------------------------------------------------------------------------------------
                static const char* dynamicRangesAvailable[] = { "SDR", "HDR", };
                uint8_t currentRangesIndex = static_cast<uint8_t>(settings->savedValues.selectedDynamicRangeMode);

                if (ImGui::BeginCombo("Dynamic Range", dynamicRangesAvailable[currentRangesIndex]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(imageFormatsAvailable); n++)
                    {
                        bool isSelected = (currentRangesIndex == n);

                        if (ImGui::Selectable(imageFormatsAvailable[n], isSelected))
                            settings->savedValues.selectedImageFormat = static_cast<EEncodingImageFormats>(n);

                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }   ImGui::SetItemTooltip("HDR requires capable image viewers that support HDR PQ.");

                ImGui::Spacing();

                // Bit depth -------------------------------------------------------------------------------------------
                static const char* bitDepthsAvailable[] = { "8", "10" };
                static const uint8_t bitDepthValues[] = { 8, 10 };

                // Determine the index based on the actual stored value
                int currentIndex = (settings->savedValues.selectedBitDepth == 10) ? 1 : 0;

                if (ImGui::BeginCombo("Bit Depth", bitDepthsAvailable[currentIndex]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(bitDepthsAvailable); n++)
                    {
                        bool isSelected = (currentIndex == n);

                        if (ImGui::Selectable(bitDepthsAvailable[n], isSelected))
                        {
                            // Assign the actual value (8 or 10), not the index
                            settings->savedValues.selectedBitDepth = bitDepthValues[n];
                        }

                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SetItemTooltip("Note: JPEG does not support 10 bits.");

                // HDR = always 10 bit
                if (settings->savedValues.selectedDynamicRangeMode == EEncodingDynamicRangeModes::HDR)
                    settings->savedValues.selectedBitDepth = 10;
                // JPG = always 8 bit and SDR
                if (settings->savedValues.selectedImageFormat == EEncodingImageFormats::jpg)
                {
                    settings->savedValues.selectedBitDepth = 8;
                    settings->savedValues.selectedDynamicRangeMode = EEncodingDynamicRangeModes::SDR;
                }

            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); // -------------------------------------------------
            ImGui::Unindent();
        }

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
