// Copyright @Dave Scholze 2026

#pragma once

#include <windows.h>
#include <filesystem>
#include <fstream>
#include <string>

#include "examples/example_glfw_vulkan/Source/Types/Structs/FSavedValues.h"

/** System to save values between sessions. */
struct LSaveSystem
{

// FUNCTIONS ===========================================================================================================
#pragma region Functions
private:

    static std::string get_settings_path()
    {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        std::filesystem::path exe_path(buffer);
        return (exe_path.parent_path() / "ffDAZmpeg.bin").string();
    }

public:

    static void saveSettings(const FSavedValues& values)
    {
        std::ofstream file(get_settings_path(), std::ios::binary);
        if (file.is_open())
        {
            file.write(reinterpret_cast<const char*>(&values), sizeof(FSavedValues));
        }
    }

    static FSavedValues loadSettings()
    {
        FSavedValues savedValues;

        std::ifstream file(get_settings_path(), std::ios::binary);
        if (file.is_open())
        {
            file.read(reinterpret_cast<char*>(&savedValues), sizeof(FSavedValues));
            return savedValues;
        }

        savedValues = FSavedValues();
        saveSettings(savedValues);
        return savedValues;
    }

    static void revertValues(FSavedValues& values)
    {
        values = FSavedValues();
        saveSettings(values);
    }

#pragma endregion

};
