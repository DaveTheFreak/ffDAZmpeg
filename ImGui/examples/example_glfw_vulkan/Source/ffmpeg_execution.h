// Copyright @Dave Scholze 2026

#pragma once

#include <thread>
#include <cstdio>
#include <string>
#include <filesystem>

#include "save_system.h"

/** This creates the actual command(s) for ffmpeg. */
struct ffmpeg_execution
{
// METHODS =============================================================================================================
#pragma region Methods

    const char* get_ffmpeg_arguments(const app_settings &settings)
    {
        ffmpeg_command.clear();

        // Input format (.png or .tiff) --------------------------------------------------------------------------------

        std::filesystem::path input_file =
            std::filesystem::path(settings.input_path) / current_filename;

        // Output format (.png or .tiff) -------------------------------------------------------------------------------

        std::filesystem::path output_file =
            std::filesystem::path(settings.output_path) / std::filesystem::path(current_filename).stem();

        switch (settings.selected_image_format)
        {
            case 0: output_file += ".avif"; break;
            case 1: output_file += ".heic"; break;
            case 2: output_file += ".jxl"; break;
            case 3: output_file += ".jpg"; break;
            default: break;
        }

        // Effects -----------------------------------------------------------------------------------------------------

        std::string chromatic_aberration_intensity = std::to_string(settings.chromatic_aberration);

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

        if (settings.logo_path[0] != '\0'
            && std::filesystem::exists(settings.logo_path)
            && std::filesystem::is_regular_file(settings.logo_path))
        {
            logo_input = " -i \"" + std::string(settings.logo_path) + "\"";

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
        std::string hdr_curve {"0/0 0.1/0.09 0.5/0.45 0.8/0.75 1/1"};
        std::string hdr_desaturation {"colorchannelmixer=rr=0.90:rg=0.05:rb=0.05:gr=0.05:gg=0.90:gb=0.05:br=0.05:bg=0.05:bb=0.90,"};

        switch (settings.selected_image_format)
        {
            case 0: // AVIF
            {
                switch (settings.selected_encoder_option)
                {
                    case 0: // NVENC
                    {
                        std::string pix_fmt = (settings.selected_bit_depth_index == 0) ? "yuv420p" : "p010le";
                        encoding = "-c:v av1_nvenc -rc constqp -qp 8 -preset p7 -pix_fmt " + pix_fmt;

                        if (settings.selected_bit_depth_index == 0 || settings.selected_dynamic_range == 0) // SDR
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
                    case 1: // SOFTWARE
                    {
                        // Software encoders typically expect standard planar formats rather than hardware semi-planar formats.
                        std::string pix_fmt = (settings.selected_bit_depth_index == 0) ? "yuv420p" : "yuv420p10le";
                        encoding = "-c:v libsvtav1 -preset 6 -crf 20 -pix_fmt " + pix_fmt;

                        if (settings.selected_bit_depth_index == 0 || settings.selected_dynamic_range == 0) // SDR
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
                    case 2: // VULKAN
                    {
                        std::string pix_fmt = (settings.selected_bit_depth_index == 0) ? "nv12" : "p010le";

                        hw_init = "-init_hw_device vulkan=vk -filter_hw_device vk ";
                        hw_upload = ",format=" + pix_fmt + ",hwupload";

                        encoding = "-c:v av1_vulkan -qp 8";

                        if (settings.selected_bit_depth_index == 0 || settings.selected_dynamic_range == 0) // SDR
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
                    default: break;
                }
                break;
            }
            case 1: // HEIC
            {
                break;
            }
            case 3: // JPEG XL
            {
                break;
            }
            case 4: // JPEG
            {
                break;
            }
            default: break;
        }

        // Image Enhancement -------------------------------------------------------------------------------------------

        std::string dither_param;

        if (settings.enable_image_enhancement)
        {
            dither_param = "-sws_dither ed ";

            int img_width = 1920;
            std::string ffprobe_path = std::string(settings.ffmpeg_path) + "\\ffprobe.exe";
            std::string probe_cmd =
                "\"\"" + ffprobe_path
            +   "\" -v error -select_streams v:0 -show_entries stream=width -of default=nw=1:nk=1 \""
            +   input_file.string() + "\"\"";

            FILE* pipe = _popen(probe_cmd.c_str(), "rt");
            if (pipe)
            {
                char buffer[128];
                if (fgets(buffer, sizeof(buffer), pipe) != nullptr)
                {
                    try { img_width = std::stoi(buffer); } catch (...) {}
                }
                _pclose(pipe);
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
    void Run(app_settings settings)
    {
        std::thread([this, settings]()
        {
            std::string local_ffmpeg_path = std::string(settings.ffmpeg_path) + "\\ffmpeg.exe";

            for (const auto& entry : std::filesystem::directory_iterator(settings.input_path))
            {
                if (entry.is_regular_file()
                    && (entry.path().extension() == ".png" || entry.path().extension() == ".tiff"))
                {
                    current_filename = entry.path().filename().string();

                    // Build the base command
                    std::string command = "\"" + local_ffmpeg_path + "\" "
                    +   get_ffmpeg_arguments(settings);

                    // Create the redirection string based on the setting
                    std::string redirection = "";
                    if (settings.generate_ffmpeg_log)
                    {
                        std::string log_file = std::string(settings.output_path) + "\\ffmpeg_log.txt";
                        redirection = " 2>> \"" + log_file + "\"";
                    }

                    // Finish the command
                    std::string finalCmd = "\"" + command + redirection + "\"";

                    // Execute command (open ffmpeg with command)
                    std::system(finalCmd.c_str());
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

