#pragma once

#include <filesystem>
#include <string>

namespace ffd::parameters
{
    static std::filesystem::path inputFile;
    static std::filesystem::path outputFile;

    static std::string chromaticAberration;
    static std::string filterChain;
    static std::string logoInput;

    static std::string hdrCurve;
    static std::string hdrDesaturation;

    static std::string hwUpload;
    static std::string hwInit;

    static std::string encoding;
    static std::string dynamicRangeParams;

    static std::string ditherParam;

    inline void reset()
    {
        chromaticAberration.clear();
        filterChain.clear();
        logoInput.clear();

        hdrCurve.clear();
        hdrDesaturation.clear();

        hwUpload.clear();
        hwInit.clear();

        encoding.clear();
        dynamicRangeParams.clear();

        ditherParam.clear();
    }

};
