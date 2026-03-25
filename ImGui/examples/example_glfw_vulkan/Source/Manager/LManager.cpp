#include "LManager.h"
// Copyright @Dave Scholze 2026

#include "LManager.h"

// METHODS =============================================================================================================
#pragma region Methods

    /** Remove all windows. */
    void LManager::emptyManager()
    {
        {
            for (auto &window : windows)
                window.reset();
            windows.clear();
        }
    }

    void LManager::updateOnBeginPlay(TSettings* settings) const
    {
        for (const auto &window : windows)
            window->OnBeginPlay(settings);
    }

    void LManager::updateTick(TSettings* settings) const
    {
        for (const auto &window : windows)
            window->Tick(settings);
    }

    void LManager::UpdateOnEndPlay(TSettings* settings) const
    {
        for (const auto &window : windows)
            window->OnEndPlay(settings);
    }

#pragma endregion
