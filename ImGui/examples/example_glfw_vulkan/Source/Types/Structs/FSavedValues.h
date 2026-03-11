// Copyright @Dave Scholze 2026

#pragma once

#include <cstdint>

#include "examples/example_glfw_vulkan/Source/Types/Enums/EEncodingAcceleration.h"
#include "examples/example_glfw_vulkan/Source/Types/Enums/EEncodingDynamicRangeModes.h"
#include "examples/example_glfw_vulkan/Source/Types/Enums/EEncodingImageFormats.h"

/** Values and Settings that are saved between sessions. */
struct FSavedValues
{
// VARIABLES ===========================================================================================================
#pragma region Variables

// VARIABLES - Folders -------------------------------------------------------------------------------------------------
#pragma region VariablesFolders

    char inputPath[512] =
        R"(C:\Users\DaveTheFreak\Nextcloud\MEDIA\NRPV\NRP-DaveLaMuerta\NRPDLM-2D\_WIP)";
    char outputPath[512] =
        R"(C:\Users\DaveTheFreak\Nextcloud\MEDIA\NRPV\NRP-DaveLaMuerta\NRPDLM-2D\_WIP)";

#pragma endregion

// VARIABLES - Image Format --------------------------------------------------------------------------------------------
#pragma region VariablesImageFormat

    /** "AVIF", "JPEG" */
    EEncodingImageFormats selectedImageFormat = EEncodingImageFormats::avif;

    /** "SDR", "HDR" */
    EEncodingDynamicRangeModes selectedDynamicRangeMode = EEncodingDynamicRangeModes::SDR;

    /** 8, 10 */
    uint8_t selectedBitDepth = 8;

#pragma endregion

// VARIABLES - Effects -------------------------------------------------------------------------------------------------
#pragma region VariablesEffects

    /** Simulated lens-based chromatic aberration intensity (0.0f = off). */
    float chromaticAberrationIntensity {0.002f};

    /** Reduces dithering, adds microdetail and improves encoding. */
    bool enableImageEnhancement {true};

    /** Optional: A logo to be put on top of the actual image, scales with its size, must have same aspect-ratio. */
    char logoPath[512] =
        R"(C:\Users\DaveTheFreak\Nextcloud\TOOLS\CGI\Daz3D\TEXTURES\DaveLaMuertaLogo.png)";

#pragma endregion

// VARIABLES - Encoder -------------------------------------------------------------------------------------------------
#pragma region VariablesEncoder

    /** Encoding acceleration for the image output. */
    EEncodingAcceleration selectEncodingAcceleration = EEncodingAcceleration::Vulkan;

#pragma endregion

// VARIABLES - Requirements --------------------------------------------------------------------------------------------
#pragma region VariablesRequirements

    /** Save ffmpeg loft into output folder. */
    bool generateFfmpegLog {false};

    /** Required: Path to the ffmpeg executable folder (bin). */
    char ffmpegExecutablePath[512] =
        R"(C:\Users\DaveTheFreak\Nextcloud\TOOLS\PICTURES\FFMPEG\ffmpeg-master-latest-win64-gpl-shared\bin)";

#pragma endregion
#pragma endregion

};
