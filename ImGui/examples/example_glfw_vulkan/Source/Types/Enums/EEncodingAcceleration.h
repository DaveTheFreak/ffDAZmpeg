// Copyright @Dave Scholze 2026

#pragma once
/** Supported image output formats. */

struct EEncodingAcceleration
{
    enum class Type : uint8_t
    {
        Vulkan,     // Vulkan encoding for all GPUs.
        Auto,       // Vendor specific encoder like NVENC for NVidia.
        Software,   // Software encoding using the CPU.
        COUNT
    };

    static constexpr const char* ComboBoxTile = "Encoder";
    static constexpr const char* NamesArray[static_cast<size_t>(Type::COUNT)] = {
        "Vulkan",
        "Auto",
        "Software"
    };
};

