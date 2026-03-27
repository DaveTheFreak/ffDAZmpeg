#pragma once

#include <filesystem>
#include <vulkan/vulkan_core.h>

#include "ffd/conversion_effects.h"
#include "ffd/parameters.h"
#include "ffd/conversion.h"

#include "imgui_impl_vulkan.h"

namespace ffd
{
    namespace g_variables
    {
        static VkAllocationCallbacks*   g_Allocator = nullptr;
        static VkInstance               g_Instance = VK_NULL_HANDLE;
        static VkPhysicalDevice         g_PhysicalDevice = VK_NULL_HANDLE;
        static VkDevice                 g_Device = VK_NULL_HANDLE;
        static uint32_t                 g_QueueFamily = (uint32_t)-1;
        static VkQueue                  g_Queue = VK_NULL_HANDLE;
        static VkPipelineCache          g_PipelineCache = VK_NULL_HANDLE;
        static VkDescriptorPool         g_DescriptorPool = VK_NULL_HANDLE;

        static ImGui_ImplVulkanH_Window g_MainWindowData;
        static uint32_t                 g_MinImageCount = 2;
        static bool                     g_SwapChainRebuild = false;
    }

    /** Create a simple drop-down menu. */
    namespace drop_down
    {
        /** Use a standard formatted enum to auto-generate a dropdown.\n
         * Example: "ffd::drop_down::draw_enum<EEncodingImageFormats>(settings->savedValues.selectedImageFormat);" */
        template<typename T>
        requires(
            std::is_enum_v<typename T::Type> &&
            !std::is_convertible_v<typename T::Type, std::underlying_type_t<typename T::Type>> &&
            std::is_same_v<std::underlying_type_t<typename T::Type>, uint8_t>)
        void draw_enum(typename T::Type& current_value)
        {
            int currentIndex = static_cast<int>(current_value);

            if (ImGui::Combo(T::ComboBoxTile, &currentIndex, T::NamesArray, static_cast<int>(T::Type::COUNT)))
            {
                current_value = static_cast<typename T::Type>(currentIndex);
            }
        }
    }

};
