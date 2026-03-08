// Copyright @Dave Scholze 2026

#pragma once

#include <thread>

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
        output_file += ".avif";

        // Effects -----------------------------------------------------------------------------------------------------

        std::string effects {""};

        // Encoder Settings --------------------------------------------------------------------------------------------

        std::string encoding;
        std::string dynamic_range;

        switch (settings.selected_image_format)
        {
            case 0: // AVIF
            {
                std::string pix_fmt = (settings.selected_bit_depth_index == 0) ? "yuv420p" : "p010le";

                encoding = "-c:v av1_nvenc -rc constqp -qp 8 -preset p7 -pix_fmt " + pix_fmt;

                if (settings.selected_bit_depth_index == 0) // 8 bit (supports only SDR)
                {
                    dynamic_range = "-color_range pc -colorspace bt709 -color_primaries bt709 -color_trc iec61966-2-1";
                }
                else // 10 bit
                {
                    if (settings.selected_dynamic_range == 0) // SDR
                    {
                        dynamic_range =
                            "-color_range pc -colorspace bt709 -color_primaries bt709 -color_trc iec61966-2-1";
                    }
                    else // HDR
                    {
                        dynamic_range = "-vf \"format=gbrp16le,curves=all='0/0 0.1/0.09 0.5/0.45 0.8/0.75 1/1',";
                        dynamic_range += "colorchannelmixer=rr=0.90:rg=0.05:rb=0.05:gr=0.05:gg=0.90:gb=0.05:br=0.05:bg=0.05:bb=0.90,";
                        dynamic_range += "zscale=pin=1:rin=1:tin=iec61966-2-1:p=9:m=9:r=0:t=16:npl=1000\" ";

                        dynamic_range += "-color_range tv -colorspace bt2020nc -color_primaries bt2020 -color_trc smpte2084 ";
                        dynamic_range += "-bsf:v av1_metadata=chroma_sample_position=1:color_range=1:color_primaries=9:transfer_characteristics=16:matrix_coefficients=9";
                    }
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

        // Actual conversion -------------------------------------------------------------------------------------------

        ffmpeg_command = "-y -i \""
        +   input_file.string() + "\" "
        +   encoding + " "
        +   dynamic_range + " \""
        +   output_file.string() + "\"";

        // Return ------------------------------------------------------------------------------------------------------
        return ffmpeg_command.c_str();
    }

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
                    std::string command = "\"" + local_ffmpeg_path + "\" " + get_ffmpeg_arguments(settings);

                    // Create the redirection string based on the setting
                    std::string redirection = "";
                    if (settings.generate_ffmpeg_log)
                    {
                        std::string log_file = std::string(settings.output_path) + "\\ffmpeg_log.txt";
                        redirection = " 2>> \"" + log_file + "\"";
                    }

                    // Final string follows your original working quote pattern
                    std::string finalCmd = "\"" + command + redirection + "\"";

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

