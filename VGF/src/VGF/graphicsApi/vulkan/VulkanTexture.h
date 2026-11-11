#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "../../RenderImpl.h"

namespace VGF::Vulkan
{
    class VulkanTexture : public TextureImpl
    {
    public:
        VulkanTexture(std::string filePath, TextureType type = TextureType::TYPE_2D);
        VulkanTexture(std::vector<std::string> filepaths, TextureType type = TextureType::TYPE_CUBE);
        VulkanTexture(const unsigned char* data, int format, unsigned int width, unsigned int height, TextureType type = TextureType::TYPE_2D);

        ImTextureID GetImGuiTexture() override;

        ~VulkanTexture();
        void* GetNativeImage() override;
    private:
        ImTextureID textureId = NULL;

        VkImage textureImage = VK_NULL_HANDLE;
        VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;
        VkImageView textureImageViewTex = VK_NULL_HANDLE;
    };
}