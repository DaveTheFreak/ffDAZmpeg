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

    void RunAlwaysFfmpegLog(app_settings settings)
    {
        // Execute in a detached background thread to prevent the DearImGui app from freezing
        std::thread([this, settings]()
        {
            std::string local_ffmpeg_path = std::string(settings.ffmpeg_path) + "\\ffmpeg.exe";

            for (const auto& entry : std::filesystem::directory_iterator(settings.input_path))
            {
                if (entry.is_regular_file()
                    && (entry.path().extension() == ".png" || entry.path().extension() == ".tiff"))
                {
                    current_filename = entry.path().filename().string();

                    std::string command = "\"" + local_ffmpeg_path + "\" " + get_ffmpeg_arguments(settings);

                    std::string log_file = std::string(settings.output_path) + "\\ffmpeg_log.txt";

                    // Wrap the entire command in an extra set of quotes for cmd.exe parsing.
                    // Use 2>> to append logs for all files in the batch.
                    std::string finalCmd = "\"" + command + " 2>> \"" + log_file + "\"\"";

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

