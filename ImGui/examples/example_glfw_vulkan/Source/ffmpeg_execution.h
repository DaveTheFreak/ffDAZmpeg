// Copyright @Dave Scholze 2026

#pragma once

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

        // Encoder Settings --------------------------------------------------------------------------------------------

        std::string encoding = R"(-c:v av1_nvenc -rc constqp -qp 8 -preset p7 -pix_fmt p010le)";

        // Effects -----------------------------------------------------------------------------------------------------

        // ...

        // Actual conversion -------------------------------------------------------------------------------------------

        ffmpeg_command = "-y -i \"" + input_file.string() + "\" " + encoding + " \"" + output_file.string() + "\"";

        // Return ------------------------------------------------------------------------------------------------------
        return ffmpeg_command.c_str();
    }

    void Run(app_settings settings)
    {
        ffmpeg_path = std::string(settings.ffmpeg_path) + "\\ffmpeg.exe";

        for (const auto& entry : std::filesystem::directory_iterator(settings.input_path))
        {
            if (entry.is_regular_file() && (entry.path().extension() == ".png" || entry.path().extension() == ".tiff"))
            {
                current_filename = entry.path().filename().string();

                std::string command = "\"" + ffmpeg_path + "\" " + get_ffmpeg_arguments(settings);

                // Added /wait to prevent launching all instances simultaneously and overloading the NVENC encoder.
                std::string finalCmd = "start \"ffmpeg_test\" /wait " + command;

                std::system(finalCmd.c_str());
            }
        }
    }

#pragma endregion

// VARIABLES ===========================================================================================================
#pragma region Variables

    std::string ffmpeg_path;
    std::string ffmpeg_command;
    std::string current_filename;

#pragma endregion

};

