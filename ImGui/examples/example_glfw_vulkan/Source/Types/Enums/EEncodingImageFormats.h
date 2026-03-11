// Copyright @Dave Scholze 2026

#pragma once

#include <cstdint>

/** Supported image output formats. */
enum class EEncodingImageFormats : uint8_t
{
    avif,   // 8 bit SDR, 10 bit SDR & 10 bit HDR
    jpg     // 8 bit SDR
};
