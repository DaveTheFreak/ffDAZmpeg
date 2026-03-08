// Copyright @Dave Scholze 2026

#pragma once

#include <windows.h>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

struct app_settings
{
    // Folders ---------------------------------------------------------------------------------------------------------
    char input_path[512] =
        R"(C:\)";
    char output_path[512] =
        R"(C:\)";

    // Format ----------------------------------------------------------------------------------------------------------

        /** "AVIF", "HEIC", "JPEG XL", "JPEG" */
        int8_t selected_image_format = 0;

        /** "SDR", "HDR" */
        int8_t selected_dynamic_range = 0;

        /** 8, 10 */
        int8_t selected_bit_depth_index = 1;

    // Effects ---------------------------------------------------------------------------------------------------------
    float chromatic_aberration {0.002f};
    bool enable_image_enhancement {true};
    char logo_path[512] =
        R"(C:\)";

    // Encoder ---------------------------------------------------------------------------------------------------------
    int8_t selected_encoder_option = 0;

    // Settings --------------------------------------------------------------------------------------------------------
    char ffmpeg_path[512] =
        R"(C:\)";

    // Logs ------------------------------------------------------------------------------------------------------------
    bool generate_ffmpeg_log {true};

};

struct save_system
{
private:

    static std::string get_settings_path()
    {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        std::filesystem::path exe_path(buffer);
        return (exe_path.parent_path() / "ffDAZmpeg.bin").string();
    }

public:

    void save_settings() const
    {
        std::ofstream file(get_settings_path(), std::ios::binary);
        if (file.is_open())
        {
            file.write(reinterpret_cast<const char*>(&settings), sizeof(app_settings));
        }
    }

    void load_settings()
    {
        std::ifstream file(get_settings_path(), std::ios::binary);
        if (file.is_open())
        {
            file.read(reinterpret_cast<char*>(&settings), sizeof(app_settings));
        }
        else
        {
            save_settings();
        }
    }

    void revert_settings()
    {
        settings = app_settings();
        save_settings();
    }

    /** Current App settings. */
    app_settings settings;

    // TODO profiles
    std::map<std::string, app_settings> profiles;

};

