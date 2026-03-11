// Copyright @Dave Scholze 2026

#pragma once

#include <cstdint>

/** Supported image output formats. */
enum class EEncodingAcceleration : uint8_t
{
    Auto,       // Vendor specific encoder like NVENC for NVidia.
    Vulkan,     // Vulkan encoding for all GPUs.
    Software    // Software encoding using the CPU.
};
