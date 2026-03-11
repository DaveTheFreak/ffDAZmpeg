#include "LManager.h"
// Copyright @Dave Scholze 2026

#include "LManager.h"

// METHODS =============================================================================================================
#pragma region Methods

    void LManager::emptyManager()
    {
        {
            for (auto &window : windows)
                window.reset();
            windows.clear();
        }
    }

void LManager::updateOnBeginPlay() const
    {
        for (const auto &window : windows)
            window->OnBeginPlay();
    }

void LManager::updateTick() const
    {
        for (const auto &window : windows)
            window->Tick();
    }

void LManager::UpdateOnEndPlay() const
    {
        for (const auto &window : windows)
            window->OnEndPlay();
    }

#pragma endregion
