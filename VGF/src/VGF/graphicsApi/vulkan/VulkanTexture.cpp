#include "VulkanTexture.h"

#include "VulkanManager.h"
#include "imgui_impl_vulkan.h"

namespace VGF::Vulkan
{
    VulkanTexture::VulkanTexture(std::string filePath, TextureType type)
    {
        Vulkan* vulkan = Vulkan::vulkan;

        int texWidth, texHeight, texChannels;

        stbi_set_flip_vertically_on_load(true);

        stbi_uc* pixels = stbi_load(filePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels) {
            Log::Error("Failed to load texture : " + filePath);
        }

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        vulkan->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(vulkan->device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(vulkan->device, stagingBufferMemory);

        stbi_image_free(pixels);

        vulkan->createImage
        (
            type, texWidth, texHeight,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            textureImage, textureImageMemory
        );

        vulkan->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, type);
        vulkan->copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), static_cast<uint8_t>(texChannels));
        vulkan->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, type);

        vkDestroyBuffer(vulkan->device, stagingBuffer, nullptr);
        vkFreeMemory(vulkan->device, stagingBufferMemory, nullptr);

        textureImageViewTex = vulkan->createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, type);
    }

    VulkanTexture::VulkanTexture(std::vector<std::string> filepaths, TextureType type)
    {
        Vulkan* vulkan = Vulkan::vulkan;

        int texWidth, texHeight, texChannels;

        if (type == TextureType::TYPE_CUBE) stbi_set_flip_vertically_on_load(false);
        else stbi_set_flip_vertically_on_load(true);

        stbi_uc* pixels = stbi_load(filepaths[0].c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;
        VkDeviceSize imagesSize = imageSize * filepaths.size();
        stbi_image_free(pixels);

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        vulkan->createBuffer((imagesSize), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(vulkan->device, stagingBufferMemory, 0, imagesSize, 0, &data);

        for (size_t i = 0; i < filepaths.size(); i++)
        {
            stbi_uc* pixels = stbi_load(filepaths[i].c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
            if (!pixels) {
                Log::Error("Failed to load texture : " + filepaths[i]);
            }

            void* dataWithOffset = static_cast<char*>(data) + imageSize * i;
            memcpy(dataWithOffset, pixels, imageSize);
            stbi_image_free(pixels);
        }

        vkUnmapMemory(vulkan->device, stagingBufferMemory);

        vulkan->createImage
        (
            type, texWidth, texHeight,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            textureImage, textureImageMemory
        );

        vulkan->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, type);
        vulkan->copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), static_cast<uint8_t>(4), 6);
        vulkan->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, type);

        vkDestroyBuffer(vulkan->device, stagingBuffer, nullptr);
        vkFreeMemory(vulkan->device, stagingBufferMemory, nullptr);

        textureImageViewTex = vulkan->createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, type);
    }

    VulkanTexture::VulkanTexture(const unsigned char* inData, int format, unsigned int width, unsigned int height, TextureType type)
    {
        Vulkan* vulkan = Vulkan::vulkan;

        VkDeviceSize imageSize = width * height * 4;
        std::vector<unsigned char> rgbaData(imageSize);

        if (format == 1)
        {
            for (unsigned int i = 0; i < width * height; i++)
            {
                rgbaData[i * 4 + 0] = inData[i];
                rgbaData[i * 4 + 1] = inData[i];
                rgbaData[i * 4 + 2] = inData[i];
                rgbaData[i * 4 + 3] = inData[i];
            }
        }
        else memcpy(rgbaData.data(), inData, imageSize);

        if (!inData) Log::Error("Failed to load texture image (data)!");


        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        vulkan->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(vulkan->device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, rgbaData.data(), static_cast<size_t>(imageSize));
        vkUnmapMemory(vulkan->device, stagingBufferMemory);

        vulkan->createImage
        (
            type, width, height,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            textureImage, textureImageMemory
        );

        vulkan->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        vulkan->copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height), static_cast<uint8_t>(4));
        vulkan->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, type);

        textureImageViewTex = vulkan->createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, type);

        vkDestroyBuffer(vulkan->device, stagingBuffer, nullptr);
        vkFreeMemory(vulkan->device, stagingBufferMemory, nullptr);
    }

    ImTextureID VulkanTexture::GetImGuiTexture()
    {
        if (textureId != NULL) return textureId;

        textureId = (ImTextureID)ImGui_ImplVulkan_AddTexture(
            Vulkan::vulkan->textureSampler,
            textureImageViewTex,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );

        if (!textureId) Log::Error("ImGuiImage not created!!");
        return textureId;
    }

    VulkanTexture::~VulkanTexture()
    {
        VkDevice& device = Vulkan::vulkan->device;
        vkDeviceWaitIdle(device);

        if (textureId)
        {
            ImGui_ImplVulkan_RemoveTexture(reinterpret_cast<VkDescriptorSet>(textureId));
            textureId = NULL;
        }

        if (textureImageViewTex != VK_NULL_HANDLE)
        {
            vkDestroyImageView(device, textureImageViewTex, nullptr);
            textureImageViewTex = VK_NULL_HANDLE;
        }

        if (textureImage != VK_NULL_HANDLE)
        {
            vkDestroyImage(device, textureImage, nullptr);
            textureImage = VK_NULL_HANDLE;
        }

        if (textureImageMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(device, textureImageMemory, nullptr);
            textureImageMemory = VK_NULL_HANDLE;
        }
    }

    void* VulkanTexture::GetNativeImage()
    {
        return static_cast<void*>(textureImageViewTex);
    }
}