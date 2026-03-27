#pragma once
#include "parameters.h"

namespace ffd::conversion
{
    namespace avif
    {

    }

    namespace jxl
    {
        inline void Encode(const TSettings* settings)
        {
            std::string pix_fmt =
                (settings->savedValues.selectedBitDepth == EEncodingBitDepth::Type::x8) ? "yuv444p" : "yuv444p10le";

            parameters::encoding = "-c:v libjxl -distance 1 -effort 7 -pix_fmt " + pix_fmt;

            // SDR -----------------------------------------------------------------------------------------------------
            if (settings->savedValues.selectedBitDepth == EEncodingBitDepth::Type::x8
                || settings->savedValues.selectedDynamicRangeMode == EEncodingDynamicRangeModes::Type::SDR)
            {
                parameters::dynamicRangeParams =
                    "-color_range pc -colorspace bt709 -color_primaries bt709 -color_trc iec61966-2-1";
            }
            // HDR -----------------------------------------------------------------------------------------------------
            else
            {
                parameters::filterChain +=
                    ",format=gbrp16le,curves=all='" + parameters::hdrCurve + "',";
                parameters::filterChain += parameters::hdrDesaturation;
                parameters::filterChain +=
                    "zscale=pin=1:rin=1:tin=iec61966-2-1:p=9:m=9:r=0:t=16:npl=1000";

                parameters::dynamicRangeParams =
                    "-color_range tv -colorspace bt2020nc -color_primaries bt2020 -color_trc smpte2084 ";
            }
        }
    }

    namespace jpg
    {
        inline void Encode()
        {
            parameters::encoding = "-c:v mjpeg -qmin 1 -qmax 1 -q:v 1 -pix_fmt yuvj444p";
            parameters::dynamicRangeParams =
                "-color_range pc -colorspace bt709 -color_primaries bt709 -color_trc iec61966-2-1";
        }
    }
};
