// Copyright @Dave Scholze 2026

#pragma once

/** Supported dynamic range modes: SDR, HDR. */
struct EEncodingDynamicRangeModes
{
    enum class Type : uint8_t
    {
        SDR,    // 8 bits
        HDR,    // 8 bits, 10 bits
        COUNT
    };

    static constexpr const char* ComboBoxTile = "Dynamic Range";
    static constexpr const char* NamesArray[static_cast<size_t>(Type::COUNT)] = {
        "SDR",
        "HDR"
    };
};


