// Copyright @Dave Scholze 2026

#pragma once

/** Values and Settings for the current runtime only. */
struct FRuntimeValues
{
    // VARIABLES ===========================================================================================================
#pragma region Variables

    // VARIABLES - Conversion ----------------------------------------------------------------------------------------------
#pragma region VariablesConversion

    /** If the app is currently converting images. */
    bool isConverting {false};

    /** Progress of image conversion in percent from 0.0f to 1.0f. */
    float conversionProgress {0.f};

#pragma endregion
#pragma endregion

};
