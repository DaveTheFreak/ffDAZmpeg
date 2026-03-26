// Copyright @Dave Scholze 2026

#pragma once

#include <thread>
#include <string>
#include <filesystem>
#include <windows.h>

#include "ffd.h"
#include "Settings/TSettings.h"
#include "Settings/SubSettings/TSavedValues.h"

/** This creates the actual command(s) for ffmpeg. */
struct FfmpegExecution
{
// METHODS =============================================================================================================
#pragma region Methods

    const char* get_ffmpeg_arguments(const TSettings* settings)
    {
        ffmpeg_command.clear();
        ffd::parameters::reset();

        // Input format (.png or .tif) --------------------------------------------------------------------------------

        ffd::parameters::inputFile =
            std::filesystem::path(settings->savedValues.inputPath) / current_filename;

        // Output format (.avif or .jpg) -------------------------------------------------------------------------------

        ffd::parameters::outputFile =
            std::filesystem::path(settings->savedValues.outputPath) / std::filesystem::path(current_filename).stem();

        switch (settings->savedValues.selectedImageFormat)
        {
            case EEncodingImageFormats::Type::avif: ffd::parameters::outputFile += ".avif"; break;
            case EEncodingImageFormats::Type::jpg: ffd::parameters::outputFile += ".jpg"; break;
        }

        // Effects -----------------------------------------------------------------------------------------------------

        ffd::conversion::effects::ChromaticAberration(settings);
        ffd::conversion::effects::LogoOverlay(settings);

        // Encoder Settings --------------------------------------------------------------------------------------------

        ffd::conversion::effects::hdr::HdrCurve(settings);
        ffd::conversion::effects::hdr::HdrDesaturation(settings);

        switch (settings->savedValues.selectedImageFormat)
        {
            case EEncodingImageFormats::Type::avif: // AVIF
            {
                switch (settings->savedValues.selectEncodingAcceleration)
                {
                    case EEncodingAcceleration::Type::Auto: // NVENC
                    {
                        std::string pix_fmt = (settings->savedValues.selectedBitDepth == EEncodingBitDepth::Type::x8) ? "yuv420p" : "p010le";
                        ffd::parameters::encoding = "-c:v av1_nvenc -rc constqp -qp 8 -preset p7 -pix_fmt " + pix_fmt;

                        if (settings->savedValues.selectedBitDepth == EEncodingBitDepth::Type::x8 || settings->savedValues.selectedDynamicRangeMode == EEncodingDynamicRangeModes::Type::SDR) // SDR
                        {
                            ffd::parameters::dynamicRangeParams =
                                "-color_range pc -colorspace bt709 -color_primaries bt709 -color_trc iec61966-2-1";
                        }
                        else // HDR
                        {
                            ffd::parameters::filterChain +=
                                ",format=gbrp16le,curves=all='" + ffd::parameters::hdrCurve + "',";
                            ffd::parameters::filterChain += ffd::parameters::hdrDesaturation;
                            ffd::parameters::filterChain +=
                                "zscale=pin=1:rin=1:tin=iec61966-2-1:p=9:m=9:r=0:t=16:npl=1000";

                            ffd::parameters::dynamicRangeParams =
                                "-color_range tv -colorspace bt2020nc -color_primaries bt2020 -color_trc smpte2084 ";
                            ffd::parameters::dynamicRangeParams +=
                                "-bsf:v av1_metadata=chroma_sample_position=1:color_range=1:color_primaries=9:transfer_characteristics=16:matrix_coefficients=9";
                        }
                        break;
                    }
                    case EEncodingAcceleration::Type::Software: // SOFTWARE
                    {
                        // Software encoders typically expect standard planar formats rather than hardware semi-planar formats.
                        std::string pix_fmt = (settings->savedValues.selectedBitDepth == EEncodingBitDepth::Type::x8) ? "yuv420p" : "yuv420p10le";
                        ffd::parameters::encoding = "-c:v libsvtav1 -preset 6 -crf 20 -pix_fmt " + pix_fmt;

                        if (settings->savedValues.selectedBitDepth == EEncodingBitDepth::Type::x8 || settings->savedValues.selectedDynamicRangeMode == EEncodingDynamicRangeModes::Type::SDR) // SDR
                        {
                            ffd::parameters::dynamicRangeParams =
                                "-color_range pc -colorspace bt709 -color_primaries bt709 -color_trc iec61966-2-1";
                        }
                        else // HDR
                        {
                            ffd::parameters::filterChain +=
                                ",format=gbrp16le,curves=all='" + ffd::parameters::hdrCurve + "',";
                            ffd::parameters::filterChain += ffd::parameters::hdrDesaturation;
                            ffd::parameters::filterChain +=
                                "zscale=pin=1:rin=1:tin=iec61966-2-1:p=9:m=9:r=0:t=16:npl=1000";

                            ffd::parameters::dynamicRangeParams =
                                "-color_range tv -colorspace bt2020nc -color_primaries bt2020 -color_trc smpte2084 ";
                            ffd::parameters::dynamicRangeParams +=
                                "-bsf:v av1_metadata=chroma_sample_position=1:color_range=1:color_primaries=9:transfer_characteristics=16:matrix_coefficients=9";
                        }
                        break;
                    }
                    case EEncodingAcceleration::Type::Vulkan: // VULKAN
                    {
                        std::string pix_fmt = (settings->savedValues.selectedBitDepth == EEncodingBitDepth::Type::x8) ? "nv12" : "p010le";

                        ffd::parameters::hwInit = "-init_hw_device vulkan=vk -filter_hw_device vk ";
                        ffd::parameters::hwUpload = ",format=" + pix_fmt + ",hwupload";

                        ffd::parameters::encoding = "-c:v av1_vulkan -qp 8";

                        if (settings->savedValues.selectedBitDepth == EEncodingBitDepth::Type::x8 || settings->savedValues.selectedDynamicRangeMode == EEncodingDynamicRangeModes::Type::SDR) // SDR
                        {
                            ffd::parameters::dynamicRangeParams =
                                "-color_range pc -colorspace bt709 -color_primaries bt709 -color_trc iec61966-2-1";
                        }
                        else // HDR
                        {
                            ffd::parameters::filterChain +=
                                ",format=gbrp16le,curves=all='" + ffd::parameters::hdrCurve + "',";
                            ffd::parameters::filterChain += ffd::parameters::hdrDesaturation;
                            ffd::parameters::filterChain +=
                                "zscale=pin=1:rin=1:tin=iec61966-2-1:p=9:m=9:r=0:t=16:npl=1000";

                            ffd::parameters::dynamicRangeParams =
                                "-color_range tv -colorspace bt2020nc -color_primaries bt2020 -color_trc smpte2084 ";
                            ffd::parameters::dynamicRangeParams +=
                                "-bsf:v av1_metadata=chroma_sample_position=1:color_range=1:color_primaries=9:transfer_characteristics=16:matrix_coefficients=9";
                        }
                        break;
                    }
                }
                break;
            }
            case EEncodingImageFormats::Type::jpg: // JPEG
            {
                ffd::parameters::encoding = "-c:v mjpeg -qmin 1 -qmax 1 -q:v 1 -pix_fmt yuvj444p";
                ffd::parameters::dynamicRangeParams =
                    "-color_range pc -colorspace bt709 -color_primaries bt709 -color_trc iec61966-2-1";
                break;
            }
        }

        // Image Enhancement -------------------------------------------------------------------------------------------

        ffd::conversion::effects::ImageEnhancement(settings);

        // Vulkan renderer post steps ----------------------------------------------------------------------------------

        ffd::parameters::filterChain += ffd::parameters::hwUpload;

        // Create final command ----------------------------------------------------------------------------------------

        ffmpeg_command =
            ffd::parameters::hwInit
        +   "-y -i \""
        +   ffd::parameters::inputFile.string() + "\" "
        +   ffd::parameters::logoInput + " "
        +   "-filter_complex \"" + ffd::parameters::filterChain + "\" "
        +   ffd::parameters::encoding + " "
        +   ffd::parameters::ditherParam
        +   ffd::parameters::dynamicRangeParams + " \""
        +   ffd::parameters::outputFile.string() + "\"";

        // Return ------------------------------------------------------------------------------------------------------
        return ffmpeg_command.c_str();
    }

    /** Run the conversions. */
    void Run(TSettings* settings)
    {
std::thread([this, settings]()
        {
            settings->runtimeValues.isConverting = true;
            settings->runtimeValues.conversionProgress = 0.0f;

            std::string local_ffmpeg_path = std::string(settings->savedValues.ffmpegExecutablePath) + "\\ffmpeg.exe";

            // 1. Count total files to calculate progress
            int total_files = 0;
            for (const auto& entry : std::filesystem::directory_iterator(settings->savedValues.inputPath))
            {
                if (entry.is_regular_file() &&
                   (entry.path().extension() == ".png" || entry.path().extension() == ".tif"))
                {
                    total_files++;
                }
            }

            if (total_files == 0)
            {
                settings->runtimeValues.conversionProgress = 1.0f;
                settings->runtimeValues.isConverting = false;
                return;
            }

            // 2. Process files
            int processed_files = 0;
            for (const auto& entry : std::filesystem::directory_iterator(settings->savedValues.inputPath))
            {
                if (entry.is_regular_file()
                    && (entry.path().extension() == ".png" || entry.path().extension() == ".tif"))
                {
                    current_filename = entry.path().filename().string();

                    // Build the base command
                    std::string command = "\"" + local_ffmpeg_path + "\" " + get_ffmpeg_arguments(settings);

                    // CreateProcessA requires a mutable buffer
                    std::vector<char> cmdBuf(command.begin(), command.end());
                    cmdBuf.push_back('\0');

                    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
                    HANDLE hLogFile = NULL;

                    if (settings->savedValues.generateFfmpegLog)
                    {
                        std::string log_file = std::string(settings->savedValues.outputPath) + "\\ffmpeg_log.txt";
                        hLogFile = CreateFileA(log_file.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ, &sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                    }

                    STARTUPINFOA si;
                    PROCESS_INFORMATION pi;
                    ZeroMemory(&si, sizeof(si));
                    si.cb = sizeof(si);

                    if (hLogFile)
                    {
                        si.dwFlags |= STARTF_USESTDHANDLES;
                        si.hStdError = hLogFile; // ffmpeg outputs logs to stderr
                        si.hStdOutput = hLogFile;
                    }

                    // Execute directly without cmd.exe to guarantee no window
                    if (CreateProcessA(NULL, cmdBuf.data(), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
                    {
                        WaitForSingleObject(pi.hProcess, INFINITE);
                        CloseHandle(pi.hProcess);
                        CloseHandle(pi.hThread);
                    }

                    if (hLogFile) CloseHandle(hLogFile);

                    // 3. Update progress after each file completes
                    processed_files++;
                    settings->runtimeValues.conversionProgress = static_cast<float>(processed_files) / static_cast<float>(total_files);
                }
            }

            // 4. Mark conversion as finished
            settings->runtimeValues.isConverting = false;

        }).detach();
    }

#pragma endregion

// VARIABLES ===========================================================================================================
#pragma region Variables

    std::string ffmpeg_path;
    std::string ffmpeg_command;
    std::string current_filename;

#pragma endregion

};

