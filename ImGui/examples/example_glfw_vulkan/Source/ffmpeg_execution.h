// Copyright @Dave Scholze 2026

#pragma once

#include <thread>
#include <string>
#include <filesystem>
#include <windows.h>

#include "Settings/TSettings.h"
#include "Settings/SubSettings/TSavedValues.h"

/** This creates the actual command(s) for ffmpeg. */
struct ffmpeg_execution
{
// METHODS =============================================================================================================
#pragma region Methods

    const char* get_ffmpeg_arguments(const TSettings* settings)
    {
        ffmpeg_command.clear();

        // Input format (.png or .tif) --------------------------------------------------------------------------------

        std::filesystem::path input_file =
            std::filesystem::path(settings->savedValues.inputPath) / current_filename;

        // Output format (.png or .tif) -------------------------------------------------------------------------------

        std::filesystem::path output_file =
            std::filesystem::path(settings->savedValues.outputPath) / std::filesystem::path(current_filename).stem();

        switch (settings->savedValues.selectedImageFormat)
        {
            case EEncodingImageFormats::Type::avif: output_file += ".avif"; break;
            case EEncodingImageFormats::Type::jpg: output_file += ".jpg"; break;
        }

        // Effects -----------------------------------------------------------------------------------------------------

        std::string chromatic_aberration_intensity = std::to_string(settings->savedValues.chromaticAberrationIntensity);

        std::string ca_logic =
            "[0:v]zscale=rin=1:r=1:tin=iec61966-2-1:t=iec61966-2-1,format=gbrp16le,extractplanes=r+g+b[r][g][b];";
        ca_logic +=
            "[r]lenscorrection=k1=" + chromatic_aberration_intensity + ":k2=0[r_dist];";
        ca_logic +=
            "[b]lenscorrection=k1=-" + chromatic_aberration_intensity + ":k2=0[b_dist];";
        ca_logic +=
            "[g][b_dist][r_dist]mergeplanes=format=gbrp16le:map0s=0:map0p=0:map1s=1:map1p=0:map2s=2:map2p=0,";
        ca_logic +=
            "zscale=rin=1:r=1:tin=iec61966-2-1:t=iec61966-2-1,crop=iw*0.995:ih*0.995,format=rgba64le";

        // Logo Overlay
        std::string filter_chain;
        std::string logo_input;

        if (settings->savedValues.logoPath[0] != '\0'
            && std::filesystem::exists(settings->savedValues.logoPath)
            && std::filesystem::is_regular_file(settings->savedValues.logoPath))
        {
            logo_input = " -i \"" + std::string(settings->savedValues.logoPath) + "\"";

            ca_logic += "[v_ca];";
            std::string logo_logic = "[v_ca]split=2[v_ca_main][v_ca_ref];";
            logo_logic += "[1:v]format=rgba64le[v_logo_raw];";
            logo_logic += "[v_logo_raw][v_ca_ref]scale=w=rw:h=rh[v_logo_scaled];";
            logo_logic += "[v_ca_main][v_logo_scaled]overlay=format=auto";

            filter_chain = ca_logic + logo_logic;
        }
        else
        {
            filter_chain = ca_logic; // Passes output directly without an output pad
        }

        // Encoder Settings --------------------------------------------------------------------------------------------

        std::string encoding;
        std::string dynamic_range_params;
        std::string hw_upload;
        std::string hw_init;

        std::string hdr_curve = std::format("0/{} 0.1/{} 0.5/{} 0.8/{} 1/{}",
            0.0f + settings->savedValues.hdrCurve[0],
            0.1f + settings->savedValues.hdrCurve[1],
            0.5f + settings->savedValues.hdrCurve[2],
            0.8f + settings->savedValues.hdrCurve[3],
            1.0f + settings->savedValues.hdrCurve[4]);

        float primary = 1.0f / 3.0f + (2.0f / 3.0f) * settings->savedValues.hdrSaturation;
        float secondary = 1.0f / 3.0f - (1.0f / 3.0f) * settings->savedValues.hdrSaturation;
        std::string hdr_desaturation = std::format(
            "colorchannelmixer=rr={0:.2f}:rg={1:.2f}:rb={1:.2f}:"
            "gr={1:.2f}:gg={0:.2f}:gb={1:.2f}:"
            "br={1:.2f}:bg={1:.2f}:bb={0:.2f},",
            primary, secondary);

        switch (settings->savedValues.selectedImageFormat)
        {
            case EEncodingImageFormats::Type::avif: // AVIF
            {
                switch (settings->savedValues.selectEncodingAcceleration)
                {
                    case EEncodingAcceleration::Type::Auto: // NVENC
                    {
                        std::string pix_fmt = (settings->savedValues.selectedBitDepth == EEncodingBitDepth::Type::x8) ? "yuv420p" : "p010le";
                        encoding = "-c:v av1_nvenc -rc constqp -qp 8 -preset p7 -pix_fmt " + pix_fmt;

                        if (settings->savedValues.selectedBitDepth == EEncodingBitDepth::Type::x8 || settings->savedValues.selectedDynamicRangeMode == EEncodingDynamicRangeModes::Type::SDR) // SDR
                        {
                            dynamic_range_params =
                                "-color_range pc -colorspace bt709 -color_primaries bt709 -color_trc iec61966-2-1";
                        }
                        else // HDR
                        {
                            filter_chain +=
                                ",format=gbrp16le,curves=all='" + hdr_curve + "',";
                            filter_chain += hdr_desaturation;
                            filter_chain +=
                                "zscale=pin=1:rin=1:tin=iec61966-2-1:p=9:m=9:r=0:t=16:npl=1000";

                            dynamic_range_params =
                                "-color_range tv -colorspace bt2020nc -color_primaries bt2020 -color_trc smpte2084 ";
                            dynamic_range_params +=
                                "-bsf:v av1_metadata=chroma_sample_position=1:color_range=1:color_primaries=9:transfer_characteristics=16:matrix_coefficients=9";
                        }
                        break;
                    }
                    case EEncodingAcceleration::Type::Software: // SOFTWARE
                    {
                        // Software encoders typically expect standard planar formats rather than hardware semi-planar formats.
                        std::string pix_fmt = (settings->savedValues.selectedBitDepth == EEncodingBitDepth::Type::x8) ? "yuv420p" : "yuv420p10le";
                        encoding = "-c:v libsvtav1 -preset 6 -crf 20 -pix_fmt " + pix_fmt;

                        if (settings->savedValues.selectedBitDepth == EEncodingBitDepth::Type::x8 || settings->savedValues.selectedDynamicRangeMode == EEncodingDynamicRangeModes::Type::SDR) // SDR
                        {
                            dynamic_range_params =
                                "-color_range pc -colorspace bt709 -color_primaries bt709 -color_trc iec61966-2-1";
                        }
                        else // HDR
                        {
                            filter_chain +=
                                ",format=gbrp16le,curves=all='" + hdr_curve + "',";
                            filter_chain += hdr_desaturation;
                            filter_chain +=
                                "zscale=pin=1:rin=1:tin=iec61966-2-1:p=9:m=9:r=0:t=16:npl=1000";

                            dynamic_range_params =
                                "-color_range tv -colorspace bt2020nc -color_primaries bt2020 -color_trc smpte2084 ";
                            dynamic_range_params +=
                                "-bsf:v av1_metadata=chroma_sample_position=1:color_range=1:color_primaries=9:transfer_characteristics=16:matrix_coefficients=9";
                        }
                        break;
                    }
                    case EEncodingAcceleration::Type::Vulkan: // VULKAN
                    {
                        std::string pix_fmt = (settings->savedValues.selectedBitDepth == EEncodingBitDepth::Type::x8) ? "nv12" : "p010le";

                        hw_init = "-init_hw_device vulkan=vk -filter_hw_device vk ";
                        hw_upload = ",format=" + pix_fmt + ",hwupload";

                        encoding = "-c:v av1_vulkan -qp 8";

                        if (settings->savedValues.selectedBitDepth == EEncodingBitDepth::Type::x8 || settings->savedValues.selectedDynamicRangeMode == EEncodingDynamicRangeModes::Type::SDR) // SDR
                        {
                            dynamic_range_params =
                                "-color_range pc -colorspace bt709 -color_primaries bt709 -color_trc iec61966-2-1";
                        }
                        else // HDR
                        {
                            filter_chain +=
                                ",format=gbrp16le,curves=all='" + hdr_curve + "',";
                            filter_chain += hdr_desaturation;
                            filter_chain +=
                                "zscale=pin=1:rin=1:tin=iec61966-2-1:p=9:m=9:r=0:t=16:npl=1000";

                            dynamic_range_params =
                                "-color_range tv -colorspace bt2020nc -color_primaries bt2020 -color_trc smpte2084 ";
                            dynamic_range_params +=
                                "-bsf:v av1_metadata=chroma_sample_position=1:color_range=1:color_primaries=9:transfer_characteristics=16:matrix_coefficients=9";
                        }
                        break;
                    }
                }
                break;
            }
            case EEncodingImageFormats::Type::jpg: // JPEG
            {
                break;
            }
        }

        // Image Enhancement -------------------------------------------------------------------------------------------

        std::string dither_param;

        if (settings->savedValues.enableImageEnhancement)
        {
            dither_param = "-sws_dither ed ";

            int img_width = 1920;
            std::string ffprobe_path = std::string(settings->savedValues.ffmpegExecutablePath) + "\\ffprobe.exe";

            // Execute ffprobe directly without cmd.exe
            std::string probe_cmd = "\"" + ffprobe_path + "\" -v error -select_streams v:0 -show_entries stream=width -of default=nw=1:nk=1 \"" + input_file.string() + "\"";

            // CreateProcessA requires a mutable buffer
            std::vector<char> probeCmdBuf(probe_cmd.begin(), probe_cmd.end());
            probeCmdBuf.push_back('\0');

            HANDLE hReadPipe, hWritePipe;
            SECURITY_ATTRIBUTES sa_pipe = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

            if (CreatePipe(&hReadPipe, &hWritePipe, &sa_pipe, 0))
            {
                STARTUPINFOA si_probe;
                PROCESS_INFORMATION pi_probe;
                ZeroMemory(&si_probe, sizeof(si_probe));
                si_probe.cb = sizeof(si_probe);
                si_probe.dwFlags = STARTF_USESTDHANDLES;
                si_probe.hStdOutput = hWritePipe;
                si_probe.hStdError = hWritePipe;
                ZeroMemory(&pi_probe, sizeof(pi_probe));

                if (CreateProcessA(NULL, probeCmdBuf.data(), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si_probe, &pi_probe))
                {
                    // Parent MUST close its copy of the write pipe immediately to prevent deadlock
                    CloseHandle(hWritePipe);

                    char buffer[128];
                    DWORD bytesRead;
                    std::string output;

                    while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0)
                    {
                        buffer[bytesRead] = '\0';
                        output += buffer;
                    }

                    WaitForSingleObject(pi_probe.hProcess, INFINITE);
                    CloseHandle(pi_probe.hProcess);
                    CloseHandle(pi_probe.hThread);

                    try { img_width = std::stoi(output); } catch (...) {}
                }
                else
                {
                    CloseHandle(hWritePipe);
                }
                CloseHandle(hReadPipe);
            }

            int deband_range = img_width / 960;
            if (deband_range < 1) deband_range = 1;

            filter_chain += ",deband=1thr=0.01:2thr=0.01:3thr=0.01:range=" + std::to_string(deband_range);
            filter_chain += ",noise=c0s=1:c0f=u";
        }

        // Vulkan renderer post steps ----------------------------------------------------------------------------------

        filter_chain += hw_upload;

        // Create final command ----------------------------------------------------------------------------------------

        ffmpeg_command =
            hw_init
        +   "-y -i \""
        +   input_file.string() + "\" "
        +   logo_input + " "
        +   "-filter_complex \"" + filter_chain + "\" "
        +   encoding + " "
        +   dither_param
        +   dynamic_range_params + " \""
        +   output_file.string() + "\"";

        // Return ------------------------------------------------------------------------------------------------------
        return ffmpeg_command.c_str();
    }

    /** Run the conversions. */
    void Run(TSettings* settings)
    {
        std::thread([this, settings]()
        {
            std::string local_ffmpeg_path = std::string(settings->savedValues.ffmpegExecutablePath) + "\\ffmpeg.exe";

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
                }
            }
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

