// Copyright @Dave Scholze 2026

#pragma once

#include <memory>
#include <vector>

#include "examples/example_glfw_vulkan/Source/LWindow.h"

class LTest;
class LWindow;

class LManager
{

// METHODS =============================================================================================================
#pragma region Methods
public:

    template <typename T, typename... Args>
    void createWindow(Args&&... args)
    {
        // Check if T is a subclass of LWindow
        static_assert(std::is_base_of<LWindow, T>::value, "T must be a subclass of LWindow");

        auto it = std::find_if(
            windows.begin(),
            windows.end(),
            [](const std::unique_ptr<LWindow>& window) {
                return dynamic_cast<T*>(window.get()) != nullptr;
        });

        if (it == windows.end())
        {
            // Create a new object of type T
            std::unique_ptr<T> newWindow = std::make_unique<T>(std::forward<Args>(args)...);

            // Add the new object to the vector
            windows.push_back(std::move(newWindow));
        }

        else
        {
            windows.erase(it);
        }

    }

    template <typename T, typename... Args>
    bool findWindow(Args&&... args)
    {
        // Check if T is a subclass of LWindow
        static_assert(std::is_base_of<LWindow, T>::value, "T must be a subclass of LWindow");

        auto it = std::find_if(
            windows.begin(),
            windows.end(),
            [](const std::unique_ptr<LWindow>& window) {
                return dynamic_cast<T*>(window.get()) != nullptr;
        });

        if (it == windows.end())
            return false;
        return true;
    }

    void emptyManager();

    void updateOnBeginPlay() const;
    void updateTick() const;
    void UpdateOnEndPlay() const;

#pragma endregion

// VARIABLES ===========================================================================================================
#pragma region Variables

    /** Currently opened windows. */
    std::vector<std::unique_ptr<LWindow>> windows;

#pragma endregion

};
