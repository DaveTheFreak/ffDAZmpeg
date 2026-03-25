// Copyright @Dave Scholze 2026

#pragma once

#include <cstdint>

/** Supported bit depths. */
struct EEncodingBitDepth
{
    enum class Type : uint8_t
    {
        x8,     // 8 bit
        x10,    // 10 bit
        COUNT
    };

    static constexpr const char* ComboBoxTile = "Bit Depth";
    static constexpr const char* NamesArray[static_cast<size_t>(Type::COUNT)] = {
        "8",
        "10"
    };
};
