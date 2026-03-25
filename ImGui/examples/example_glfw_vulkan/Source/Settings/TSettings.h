// Copyright @Dave Scholze 2026

#pragma once

#include <fstream>
#include <string>


#include <thread>
#include <filesystem>
#include <windows.h>

#include "SubSettings/TRuntimeValues.h"
#include "SubSettings/TSavedValues.h"

/** Values and Settings for the current runtime only. */
struct TSettings
{
// VARIABLES ===========================================================================================================
#pragma region Variables

    /** Temporary runtime values/settings. */
    TRuntimeValues runtimeValues;

    /** Permanently stored values/settings. */
    TSavedValues savedValues;

#pragma endregion

// METHODS =============================================================================================================
#pragma region Methods
private:

    static std::string get_settings_path()
    {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        std::filesystem::path exe_path(buffer);
        return (exe_path.parent_path() / "ffDAZmpeg.bin").string();
    }

public:

    void saveSettings() const
    {
        std::ofstream file(get_settings_path(), std::ios::binary);
        if (file.is_open())
        {
            file.write(reinterpret_cast<const char*>(&savedValues), sizeof(TSavedValues));
        }
    }

    void loadSettings()
    {
        TSavedValues loadedValues;

        std::ifstream file(get_settings_path(), std::ios::binary);
        if (file.is_open())
        {
            file.read(reinterpret_cast<char*>(&loadedValues), sizeof(TSavedValues));
            savedValues = loadedValues;
        }

        loadedValues = TSavedValues();
        saveSettings();
    }

    void revertValues()
    {
        savedValues = TSavedValues();
        saveSettings();
    }

#pragma endregion

};
