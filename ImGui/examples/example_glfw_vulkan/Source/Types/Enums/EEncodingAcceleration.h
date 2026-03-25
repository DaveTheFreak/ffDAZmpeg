// Copyright @Dave Scholze 2026

#pragma once
/** Supported image output formats. */

struct EEncodingAcceleration
{
    enum class Type : uint8_t
    {
        Auto,       // Vendor specific encoder like NVENC for NVidia.
        Vulkan,     // Vulkan encoding for all GPUs.
        Software,   // Software encoding using the CPU.
        COUNT
    };

    static constexpr const char* ComboBoxTile = "Encoder";
    static constexpr const char* NamesArray[static_cast<size_t>(Type::COUNT)] = {
        "Auto",
        "Vulkan",
        "Software"
    };
};

