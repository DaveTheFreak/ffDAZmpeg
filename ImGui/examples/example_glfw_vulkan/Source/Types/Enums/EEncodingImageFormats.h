// Copyright @Dave Scholze 2026

#pragma once

/** Supported image output formats. */
struct EEncodingImageFormats
{
    enum class Type : uint8_t
    {
        avif,   // 8 bit SDR, 10 bit SDR & 10 bit HDR
        jxl,    // 8 bit SDR, 10 bit SDR & 10 bit HDR
        jpg,    // 8 bit SDR
        COUNT
    };

    static constexpr const char* ComboBoxTile = "Format";
    static constexpr const char* NamesArray[static_cast<size_t>(Type::COUNT)] = {
        "AVIF",
        "JPEG XL",
        "JPG"
    };
};
