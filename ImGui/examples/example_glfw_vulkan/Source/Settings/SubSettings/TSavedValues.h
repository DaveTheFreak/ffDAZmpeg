// Copyright @Dave Scholze 2026

#pragma once

#include "examples/example_glfw_vulkan/Source/Types/Enums/EEncodingAcceleration.h"
#include "examples/example_glfw_vulkan/Source/Types/Enums/EEncodingBitDepth.h"
#include "examples/example_glfw_vulkan/Source/Types/Enums/EEncodingDynamicRangeModes.h"
#include "examples/example_glfw_vulkan/Source/Types/Enums/EEncodingImageFormats.h"

/** Values and Settings that are saved between sessions. */
struct TSavedValues
{
// VARIABLES ===========================================================================================================
#pragma region Variables

// VARIABLES - Folders -------------------------------------------------------------------------------------------------
#pragma region VariablesFolders

    char inputPath[512] =
        R"(C:\Users\DaveTheFreak\Desktop\DAZ)";
    char outputPath[512] =
        R"(C:\Users\DaveTheFreak\Desktop\DAZ)";

#pragma endregion

// VARIABLES - Image Format --------------------------------------------------------------------------------------------
#pragma region VariablesImageFormat

    /** "AVIF", "JPEG" */
    EEncodingImageFormats::Type selectedImageFormat = EEncodingImageFormats::Type::avif;

    /** "SDR", "HDR" */
    EEncodingDynamicRangeModes::Type selectedDynamicRangeMode = EEncodingDynamicRangeModes::Type::SDR;

    /** 8, 10 */
    EEncodingBitDepth::Type selectedBitDepth = EEncodingBitDepth::Type::x8;

#pragma endregion

// VARIABLES - Effects -------------------------------------------------------------------------------------------------
#pragma region VariablesEffects

    /** Simulated lens-based chromatic aberration intensity (0.0f = off). */
    float chromaticAberrationIntensity {0.0021f};

    /** Reduces dithering, adds microdetail and improves encoding. */
    bool enableImageEnhancement {true};

    /** HDR mapping curve for various brightness levels. */
    float hdrCurve[5] = {0.f, -0.02f, -0.04f, -0.06f, 0.f};

    /** Image saturation. */
    float hdrSaturation {0.85f};

    /** Optional: A logo to be put on top of the actual image, scales with its size, must have same aspect-ratio. */
    char logoPath[512] =
        R"(C:\Users\DaveTheFreak\Desktop\DAZ\Logo\MyLogo.png)";

#pragma endregion

// VARIABLES - Encoder -------------------------------------------------------------------------------------------------
#pragma region VariablesEncoder

    /** Encoding acceleration for the image output. */
    EEncodingAcceleration::Type selectEncodingAcceleration = EEncodingAcceleration::Type::Vulkan;

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
