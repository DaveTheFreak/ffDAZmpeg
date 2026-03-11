// Copyright @Dave Scholze 2026

#pragma once

#include <cstdint>

/** Supported dynamic range modes: SDR, HDR. */
enum class EEncodingDynamicRangeModes : uint8_t
{
    SDR,   // 8 bits
    HDR    // 8 bits, 10 bits
};
