#include "VulkanManager.h"

#include <ranges>

#include "imgui_impl_vulkan.h"
#include "../../Camera.h"

namespace VGF::Vulkan
{
    std::vector<RenderData> opaqueSwapchainObjects;
    std::vector<RenderData> translucentSwapchainObjects;
    
    VulkanTexture::VulkanTexture(std::string filePath)
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
            texWidth, texHeight,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            textureImage, textureImageMemory);

        vulkan->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        vulkan->copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
        vulkan->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(vulkan->device, stagingBuffer, nullptr);
        vkFreeMemory(vulkan->device, stagingBufferMemory, nullptr);

        textureImageViewTex = vulkan->createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
    }
    VulkanTexture::VulkanTexture(const unsigned char* inData, int format, unsigned int width, unsigned int height)
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
        else
            memcpy(rgbaData.data(), inData, imageSize);

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
            width, height,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            textureImage, textureImageMemory
        );

        vulkan->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        vulkan->copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
        vulkan->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        textureImageViewTex = vulkan->createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

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

    void VulkanTexture::Bind(textureType type)
    {
        switch (type)
        {
        case textureType::color:
            Vulkan::vulkan->colorTextureImageView = textureImageViewTex;
            break;
        case textureType::metallicRoughness:
            Vulkan::vulkan->metallicRoughnessTextureImageView = textureImageViewTex;
            break;
        case textureType::emissive:
            Vulkan::vulkan->emissiveTextureImageView = textureImageViewTex;
            break;
        case textureType::occulsion:
            Vulkan::vulkan->occulsionTextureImageView = textureImageViewTex;
            break;
        case textureType::normal:
            Vulkan::vulkan->normalTextureImageView = textureImageViewTex;
            break;
        default:
            Vulkan::vulkan->colorTextureImageView = textureImageViewTex;
            break;
        }
    }

    void* VulkanTexture::GetNativeImage()
    {
        return (void*)textureImageViewTex;
    }

#if defined(_DEBUG) || defined(DEBUGRELEASE)
    const bool enableValidationLayers = true;
#else
    const bool enableValidationLayers = false;
#endif
    
    const std::vector<const char*> validationLayers = 
    {
        "VK_LAYER_KHRONOS_validation"
    };
    const std::vector<const char*> deviceExtensions = 
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    Vulkan* Vulkan::vulkan = nullptr;

    Vulkan::Vulkan(GLFWwindow* window)
    {
        Vulkan::vulkan = this;

        this->window = window;

        createInstance();
        setupDebugMessenger();
        createSurface(window);
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
    
        createImageViews(swapChainImageViews, swapChainImages);

        createRenderPass();
        createOffscreenRenderPass();
        createCommandPool();
        createDepthResources();

        createFramebuffers(swapChainFramebuffers, swapChainImageViews, false);

        createTextureSampler();
        createCommandBuffers();
        createSyncObjects();
    }
    Vulkan::~Vulkan()
    {
        // Wait vulkan to be ready
        vkDeviceWaitIdle(device);

        //cleanup -----------------------
        cleanupSwapChain();

        vkDestroyRenderPass(device, renderPass, nullptr);
        vkDestroyRenderPass(device, offscreenRenderPass, nullptr);

        vkDestroySampler(device, textureSampler, nullptr);

        // if (colorTextureImageView != VK_NULL_HANDLE) vkDestroyImageView(device, colorTextureImageView, nullptr);
        // if (metallicRoughnessTextureImageView != VK_NULL_HANDLE) vkDestroyImageView(device, metallicRoughnessTextureImageView, nullptr);
        // if (emissiveTextureImageView != VK_NULL_HANDLE) vkDestroyImageView(device, emissiveTextureImageView, nullptr);
        // if (occulsionTextureImageView != VK_NULL_HANDLE) vkDestroyImageView(device, occulsionTextureImageView, nullptr);
        // if (normalTextureImageView != VK_NULL_HANDLE) vkDestroyImageView(device, normalTextureImageView, nullptr);

        for (PipelineData& pipelineData : pipelineCache | std::views::values)
        {
            vkDestroyPipeline(device, pipelineData.pipeline, nullptr);
            vkDestroyPipelineLayout(vulkan->device, pipelineData.pipelineLayout, nullptr);
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, inFlightFences[i], nullptr);
        }

        for (size_t i = 0; i < swapChainImages.size(); i++)
        {
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        }

        vkDestroyCommandPool(device, commandPool, nullptr);

        vkDestroyDevice(device, nullptr);

        if (enableValidationLayers) DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);
    }

    void Vulkan::cleanupSwapChain() 
    {
        vkDestroyImageView(device, depthImageView, nullptr);
        vkDestroyImage(device, depthImage, nullptr);
        vkFreeMemory(device, depthImageMemory, nullptr);

        for (VkFramebuffer& framebuffer : swapChainFramebuffers) vkDestroyFramebuffer(device, framebuffer, nullptr);
        for (VkImageView& imageView : swapChainImageViews) vkDestroyImageView(device, imageView, nullptr);

        vkDestroySwapchainKHR(device, swapChain, nullptr);
    }
    void Vulkan::recreateSwapChain() 
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        while (width == 0 || height == 0) 
        {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(device);
        cleanupSwapChain();
        createSwapChain();
        createImageViews(swapChainImageViews, swapChainImages);
        createDepthResources();
        createFramebuffers(swapChainFramebuffers, swapChainImageViews, false);

        for (VulkanFrameBuffer* framebuffer : allFramebuffers) framebuffer->Recreate();
    }

    VkResult Vulkan::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) 
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        else return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
    void Vulkan::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) 
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) func(instance, debugMessenger, pAllocator);
    }

    struct Vulkan::QueueFamilyIndices 
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() 
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct Vulkan::SwapChainSupportDetails 
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    void Vulkan::pickPhysicalDevice() 
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            VGF::Log::Error("Failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto& device : devices) 
        {
            if (isDeviceSuitable(device)) 
            {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            VGF::Log::Error("Failed to find a suitable GPU!");
        }
    }
    bool Vulkan::isDeviceSuitable(VkPhysicalDevice device) 
    {
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) 
        {
            Vulkan::SwapChainSupportDetails swapChainSupport = Vulkan::querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
    }

    bool Vulkan::checkDeviceExtensionSupport(VkPhysicalDevice device) 
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiVGFExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions)
            requiVGFExtensions.erase(extension.extensionName);

        return requiVGFExtensions.empty();
    }
    Vulkan::QueueFamilyIndices Vulkan::findQueueFamilies(VkPhysicalDevice device) 
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) 
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
                indices.graphicsFamily = i;

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport)
                indices.presentFamily = i;

            if (indices.isComplete())
                break;

            i++;
        }

        return indices;
    }
    void Vulkan::createSurface(GLFWwindow* window) 
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            VGF::Log::Error("Failed to create window surface!");
        }
    }

    void Vulkan::renderEnd()
    {
        vkDeviceWaitIdle(device);
    }
    void Vulkan::render()
    {
        drawFrame();
    }

    void Vulkan::createLogicalDevice() 
    {

        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) 
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers) 
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else createInfo.enabledLayerCount = 0;

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            VGF::Log::Error("Failed to create logical device!");
        }

        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
    }

    void Vulkan::createSwapChain() {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
            imageCount = swapChainSupport.capabilities.maxImageCount;

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        if (indices.graphicsFamily != indices.presentFamily) 
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            VGF::Log::Error("Failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }
    VkSurfaceFormatKHR Vulkan::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
    {
        for (const auto& availableFormat : availableFormats) 
        {
            if (availableFormat.format == VK_FORMAT_R8G8B8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return availableFormat;
        }

        return availableFormats[0];
    }
    VkPresentModeKHR Vulkan::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) 
    {
        for (const auto& availablePresentMode : availablePresentModes) 
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) return availablePresentMode;
            if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) return availablePresentMode;
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }
    VkExtent2D Vulkan::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) 
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
            return capabilities.currentExtent;
        else 
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = 
            {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }
    Vulkan::SwapChainSupportDetails Vulkan::querySwapChainSupport(VkPhysicalDevice device) 
    {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) 
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) 
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    void Vulkan::createImages(std::vector<VkImage>& images, std::vector<VkDeviceMemory>& imageMemory)
    {
        images.resize(swapChainImages.size());
        imageMemory.resize(swapChainImages.size());

        for (uint32_t i = 0; i < swapChainImages.size(); i++)
        {
            VkImageCreateInfo imageCreateCI{};
            imageCreateCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageCreateCI.imageType = VK_IMAGE_TYPE_2D;
            imageCreateCI.format = swapChainImageFormat;
            imageCreateCI.extent.width = swapChainExtent.width;
            imageCreateCI.extent.height = swapChainExtent.height;
            imageCreateCI.extent.depth = 1;
            imageCreateCI.mipLevels = 1;
            imageCreateCI.arrayLayers = 1;
            imageCreateCI.samples = VK_SAMPLE_COUNT_1_BIT;
            imageCreateCI.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageCreateCI.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            imageCreateCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageCreateCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            if (vkCreateImage(device, &imageCreateCI, nullptr, &images[i]) != VK_SUCCESS) {
                VGF::Log::Error("Failed to create vkImage!");
            }

            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(device, images[i], &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory[i]) != VK_SUCCESS) {
                VGF::Log::Error("Failed to allocate memory for vkImage!");
            }

            vkBindImageMemory(device, images[i], imageMemory[i], 0);
        }
    }

    void Vulkan::createImageViews(std::vector<VkImageView>& imageViews, const std::vector<VkImage> images)
    {
        imageViews.resize(images.size());

        for (size_t i = 0; i < images.size(); i++)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device, &createInfo, nullptr, &imageViews[i]) != VK_SUCCESS) {
                VGF::Log::Error("Failed to create image views!");
            }
        }
    }

    void Vulkan::createDescriptorSetLayout(VkDescriptorSetLayout& descriptorsetLayout, std::vector<VulkanUniformBuffer>& uniformBuffers, std::vector<VulkanImageSampler>& imageSamplers)
    {
        size_t bufferSize = uniformBuffers.size();
        size_t samplerSize = imageSamplers.size();
        std::vector<VkDescriptorSetLayoutBinding> bindings{};
        bindings.resize(bufferSize + samplerSize);

        for (size_t i = 0; i < bufferSize; i++)
        {
            bindings[i].binding = i;
            bindings[i].descriptorCount = 1;
            bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            bindings[i].pImmutableSamplers = nullptr;
            bindings[i].stageFlags = uniformBuffers[i].uniformBufferObject->getShaderStage() == 
                ShaderStage::VERTEX ?
                VK_SHADER_STAGE_VERTEX_BIT :
                VK_SHADER_STAGE_FRAGMENT_BIT;
        }

        for (size_t i = 0; i < samplerSize; i++)
        {
            bindings[bufferSize + i].binding = bufferSize + i;
            bindings[bufferSize + i].descriptorCount = 1;
            bindings[bufferSize + i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            bindings[bufferSize + i].pImmutableSamplers = nullptr;
            bindings[bufferSize + i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorsetLayout) != VK_SUCCESS) {
            VGF::Log::Error("Failed to create descriptor set layout!");
        }
    }
    void Vulkan::createDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, VkDescriptorSetLayout layout, VkDescriptorPool descriptorPool, std::vector<VulkanUniformBuffer>& vulkanUniformBuffers, std::vector<VulkanImageSampler>& imageSamplers)
    {
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, layout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
            VGF::Log::Error("Failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
            std::vector<VkDescriptorBufferInfo> bufferInfoArray{};
            for (auto buffer : vulkanUniformBuffers)
            {
                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = buffer.uniformBuffers[i];
                bufferInfo.offset = 0;
                bufferInfo.range = buffer.uniformBufferObject->SizeOf();

                bufferInfoArray.push_back(bufferInfo);
            }

            std::vector<VkDescriptorImageInfo> samplerInfoArray{};
            for (auto sampler : imageSamplers)
            {
                VkDescriptorImageInfo samplerInfo{};
                samplerInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                samplerInfo.imageView = sampler.imageViews[i % sampler.imageViews.size()];
                samplerInfo.sampler = sampler.textureSampler;

                samplerInfoArray.push_back(samplerInfo);
            }


            size_t bufferSize = vulkanUniformBuffers.size();
            size_t samplerSize = imageSamplers.size();

            std::vector<VkWriteDescriptorSet> descriptorWrites{};
            descriptorWrites.resize(bufferSize + samplerSize);

            for (size_t b = 0; b < bufferSize; b++)
            {
                descriptorWrites[b].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[b].dstSet = descriptorSets[i];
                descriptorWrites[b].dstBinding = b;
                descriptorWrites[b].dstArrayElement = 0;
                descriptorWrites[b].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[b].descriptorCount = 1;
                descriptorWrites[b].pBufferInfo = &bufferInfoArray[b];
            }

            for (size_t s = 0; s < samplerSize; s++)
            {
                descriptorWrites[bufferSize + s].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[bufferSize + s].dstSet = descriptorSets[i];
                descriptorWrites[bufferSize + s].dstBinding = bufferSize + s;
                descriptorWrites[bufferSize + s].dstArrayElement = 0;
                descriptorWrites[bufferSize + s].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrites[bufferSize + s].descriptorCount = 1;
                descriptorWrites[bufferSize + s].pImageInfo = &samplerInfoArray[s];
            }

            vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    unsigned int Vulkan::getOrCreatePipeline(VulkanRenderer* object, const PipelineConfig& config, const bool offscreen)
    {
        PipelineHashKey key{ config, offscreen };
        auto it = pipelineCache.find(key);
        if (it != pipelineCache.end())
        {
            return it->second.index;
        }

        PipelineData pipelineData = createGraphicsPipeline(object, config, offscreen ? offscreenRenderPass : renderPass);

        pipelineData.index = pipelineCache.size();
        pipelineData.offscreen = offscreen;

        pipelines.emplace_back(std::pair{ pipelineData.pipeline, pipelineData.pipelineLayout });

        pipelineCache[key] = pipelineData;

        return pipelineData.index;
    }

    VkPipeline& Vulkan::GetPipeline(unsigned int pipelineID)
    {
        if (pipelineID > pipelines.size()) Log::Error("Could not find pipeline : " + std::to_string(pipelineID));
        return pipelines.at(pipelineID).first;
    }

    VkPipelineLayout& Vulkan::GetPipelineLayout(unsigned int pipelineID)
    {
        if (pipelineID > pipelines.size()) Log::Error("Could not find pipelinelayout : " + std::to_string(pipelineID));
        return pipelines.at(pipelineID).second;
    }

    PipelineData Vulkan::createGraphicsPipeline(VulkanRenderer* object, const PipelineConfig& config, VkRenderPass& renderPass)
    {
        VkPipeline pipeline;
        VkPipelineLayout pipelineLayout;

        VulkanGraphicsPipeline graphicsPipeline
        (
            config,
            device,
            object->descriptorSetLayout,
            renderPass,
            pipelineLayout,
            pipeline
        );

        return {pipeline, pipelineLayout};
    }

    void Vulkan::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            VGF::Log::Error("Failed to begin recording command buffer!");
        }
        
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderArea.offset = { 0, 0 };
        

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        VkDeviceSize offsets[] = { 0 };

        for (size_t i = 0; i < framebuffers.size(); i++)
        {
            renderPassInfo.renderArea.extent = framebuffers[i]->extent;

            viewport.y = (float)framebuffers[i]->extent.height;
            viewport.width = (float)framebuffers[i]->extent.width;
            viewport.height = -(float)framebuffers[i]->extent.height;

            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

            const bool offscreen = framebuffers[i]->GetOffscreen();
            renderPassInfo.renderPass = offscreen ? offscreenRenderPass : renderPass;
            renderPassInfo.framebuffer = framebuffers[i]->framebuffer;

            vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            for (RenderData& data : framebuffers[i]->opaqueObjects)
                data.renderer->Draw(commandBuffer, currentFrame, data);

            for (RenderData& data : framebuffers[i]->translucentObjects)
                data.renderer->Draw(commandBuffer, currentFrame, data);

            framebuffers[i]->opaqueObjects.clear();
            framebuffers[i]->translucentObjects.clear();

            vkCmdEndRenderPass(commandBuffer);
        }

        framebuffers.clear();

        renderPassInfo.renderArea.extent = swapChainExtent;

        viewport.y = (float)swapChainExtent.height;
        viewport.width = (float)swapChainExtent.width;
        viewport.height = -(float)swapChainExtent.height;

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        for (RenderData& data : opaqueSwapchainObjects)
            data.renderer->Draw(commandBuffer, currentFrame, data);

        for (RenderData& data : translucentSwapchainObjects)
            data.renderer->Draw(commandBuffer, currentFrame, data);

        opaqueSwapchainObjects.clear();
        translucentSwapchainObjects.clear();

        // Imgui Render
        if (vulkanGui) VulkanGui::Render(commandBuffer);

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            VGF::Log::Error("Failed to record command buffer!");
        }
    }
    void Vulkan::createCommandPool() {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            VGF::Log::Error("Failed to create command pool!");
        }
    }

    void Vulkan::createDepthResources() 
    {
        VkFormat depthFormat = findDepthFormat();

        createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
        depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    }
    VkFormat Vulkan::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
    {
        for (VkFormat format : candidates) 
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        VGF::Log::Error("Failed to find supported format!");
        return VK_FORMAT_UNDEFINED;
    }
    VkFormat Vulkan::findDepthFormat() const
    {
        return findSupportedFormat(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }
    bool Vulkan::hasStencilComponent(VkFormat format) 
    {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    void Vulkan::createCommandBuffers() 
    {
        commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

        if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            VGF::Log::Error("Failed to allocate command buffers!");
        }
    }

    VkDescriptorPool Vulkan::createDescriptorPool(VkDescriptorPool& descriptorPool, size_t uniformBufferCount, size_t imageSamplerCount, unsigned int maxSets)
    {
        std::vector<VkDescriptorPoolSize> poolSizes{};
        poolSizes.resize(uniformBufferCount + imageSamplerCount);

        for (size_t i = 0; i < uniformBufferCount; i++)
        {
            poolSizes[i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[i].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        }

        for (size_t i = 0; i < imageSamplerCount; i++)
        {
            poolSizes[uniformBufferCount + i].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[uniformBufferCount + i].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        }

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * maxSets);

        if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            VGF::Log::Error("Failed to create descriptor pool!");
        }

        return descriptorPool;
    }

    void Vulkan::createSyncObjects() 
    {
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(swapChainImages.size());
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS)
            {
                VGF::Log::Error("Failed to create synchronization objects for a frame!");
            }
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if 
            (
                vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS
            ) 
            {
                VGF::Log::Error("Failed to create synchronization objects for a frame!");
            }
        }
    }
    void Vulkan::drawFrame() {
        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            VGF::Log::Error("Failed to acquire swap chain image!");
        }

        vkResetFences(device, 1, &inFlightFences[currentFrame]);

        vkResetCommandBuffer(commandBuffers[currentFrame], 0);
        recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

        std::array<VkCommandBuffer, 1> submitCommandBuffers;
        submitCommandBuffers = {commandBuffers[currentFrame]};

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = submitCommandBuffers.data();

        VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[imageIndex] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
            VGF::Log::Error("Failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
            framebufferResized = false;
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            VGF::Log::Error("Failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Vulkan::createFramebuffers(std::vector<VkFramebuffer>& framebuffers, const std::vector<VkImageView> imageViews, bool offscreen)
    {
        framebuffers.resize(imageViews.size());
        for (size_t i = 0; i < imageViews.size(); i++)
        {
            std::array<VkImageView, 2> attachments =
            {
                imageViews[i],
                depthImageView
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = offscreen ? offscreenRenderPass : renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
                VGF::Log::Error("Failed to create framebuffer!");
            }
        }
    }

    void Vulkan::createRenderPass() 
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        std::array<VkSubpassDependency ,2> dependencies{};
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependencies[0].srcAccessMask = 0;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].dstSubpass = 0;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
        renderPassInfo.pDependencies = dependencies.data();

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            VGF::Log::Error("Failed to create render pass!");
        }
    }

    void Vulkan::createOffscreenRenderPass() 
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;


        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        std::array<VkSubpassDependency, 2> dependencies;

        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_NONE_KHR;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


        std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
        renderPassInfo.pDependencies = dependencies.data();

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &offscreenRenderPass) != VK_SUCCESS) {
            VGF::Log::Error("Failed to create render pass!");
        }
    }
    void Vulkan::createTextureSampler() 
    {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {         
            VGF::Log::Error("Failed to create texture sampler!");
        }
    }
    VkImageView Vulkan::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) 
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            VGF::Log::Error("Failed to create texture image view!");
        }

        return imageView;
    }

    void Vulkan::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) 
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
            VGF::Log::Error("Failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
            VGF::Log::Error("Failed to allocate image memory!");
        }

        vkBindImageMemory(device, image, imageMemory, 0);
    }
    void Vulkan::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) 
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else {
            VGF::Log::Error("Unsupported layout transition!");
        }

        vkCmdPipelineBarrier
        (
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        endSingleTimeCommands(commandBuffer);
    }
    void Vulkan::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) 
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = {
            width,
            height,
            1
        };

        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        endSingleTimeCommands(commandBuffer);
    }

    void Vulkan::createVertexBuffer(VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory, std::vector<GLfloat>& vertices)
    {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t)bufferSize);

        vkUnmapMemory(device, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
        copyBuffer(stagingBuffer, vertexBuffer, bufferSize);


        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }

    void Vulkan::createInstanceBuffer(VkBuffer& instanceBuffer, VkDeviceMemory& instanceBufferMemory, VkDeviceSize size)
    {
        createBuffer
        (
            size,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            instanceBuffer,
            instanceBufferMemory
        );
    }

    void Vulkan::createIndexBuffer(VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory, std::vector<uint32_t> indices)
    {

        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t)bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

        copyBuffer(stagingBuffer, indexBuffer, bufferSize);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }

    void Vulkan::createUniformBuffers(VulkanUniformBuffer& buffer, size_t typeSize) 
    {
        VkDeviceSize bufferSize = typeSize;

        buffer.uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        buffer.uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
        buffer.uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
            createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                buffer.uniformBuffers[i], buffer.uniformBuffersMemory[i]);

            vkMapMemory(device, buffer.uniformBuffersMemory[i], 0, bufferSize, 0, &buffer.uniformBuffersMapped[i]);
        }
    }

    VkCommandBuffer Vulkan::beginSingleTimeCommands() const
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }
    void Vulkan::endSingleTimeCommands(VkCommandBuffer commandBuffer) const
    {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);

        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }

    void Vulkan::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) 
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {        
            VGF::Log::Error("Failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            VGF::Log::Error("Failed to allocate buffer memory!");
        }

        vkBindBufferMemory(device, buffer, bufferMemory, 0);
    }
    void Vulkan::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) 
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);

        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }

    uint32_t Vulkan::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        VGF::Log::Error("Failed to find suitable memory type!");
        return 0;
    }

    void Vulkan::createInstance() 
    {
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            VGF::Log::Error("Validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello World";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "VGFrame";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers) 
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else 
        {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            VGF::Log::Error("Failed to create instance!");
        }
    }
    void Vulkan::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) 
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }
    void Vulkan::setupDebugMessenger() 
    {
        if (!enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            VGF::Log::Error("Failed to set up debug messenger!");
        }
    }

    std::vector<const char*> Vulkan::getRequiredExtensions() 
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }
    bool Vulkan::checkValidationLayerSupport()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers)
        {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound){
                return false;
            }
        }

        return true;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) 
    {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    std::vector<uint16_t> convertIndices(const std::vector<GLuint>& indices) 
    {
        std::vector<uint16_t> converted;
        converted.reserve(indices.size());  // Reserve space for efficiency

        for (GLuint index : indices) 
        {
            if (index > std::numeric_limits<uint16_t>::max())
                converted.push_back(std::numeric_limits<uint16_t>::max());
            else 
                converted.push_back(static_cast<uint16_t>(index));
        }

        return converted;
    }

    void Vulkan::UpdateTexture(std::vector<VkDescriptorSet> descriptorSets, VkImageView& lastTexture, VkImageView imageView, uint32_t binding)
    {
        if (lastTexture != imageView)
        {
            for (size_t i = 0; i < descriptorSets.size(); i++)
            {
                //vkDeviceWaitIdle(device); // TODO : not checked if better way to do this.
                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = imageView;
                imageInfo.sampler = textureSampler;

                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = descriptorSets[i];
                descriptorWrite.dstBinding = binding; // Ensure it matches shader binding
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pImageInfo = &imageInfo;

                vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
            }

            lastTexture = imageView;
        }
    }

    std::vector<GLfloat> defaultVertices =
    {
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
        1.0f, -1.0f, 0.0f,    0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,    0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f
    };

    std::vector<GLuint> defaultIndices =
    {
        0, 1, 2,
        2, 3, 0
    };

    VulkanRenderer::VulkanRenderer(std::vector<GLuint>& indices, std::vector<GLfloat>& vertices, std::vector<UniformBufferObject*> uniformBuffers)
    {
        vulkan = Vulkan::vulkan;

        vulkan->createVertexBuffer(vertexBuffer, vertexBufferMemory, vertices);
        vulkan->createIndexBuffer(indexBuffer, indexBufferMemory, indices);

        for (UniformBufferObject* buffer : uniformBuffers)
        {
            VulkanUniformBuffer vulkanBuffer;
            vulkanBuffer.uniformBufferObject = buffer;
            vulkan->createUniformBuffers(vulkanBuffer, buffer->SizeOf());
            vulkanUniformBuffers.emplace_back(vulkanBuffer);
        }

        std::vector<VulkanImageSampler> samplers =
        {
            {{vulkan->colorTextureImageView}, vulkan->textureSampler},
            {{vulkan->metallicRoughnessTextureImageView}, vulkan->textureSampler},
            {{vulkan->emissiveTextureImageView}, vulkan->textureSampler},
            {{vulkan->occulsionTextureImageView}, vulkan->textureSampler},
            {{vulkan->normalTextureImageView}, vulkan->textureSampler},
        };

        vulkan->createDescriptorSetLayout(descriptorSetLayout, vulkanUniformBuffers, samplers);
        vulkan->createDescriptorPool(descriptorPool, vulkanUniformBuffers.size(), samplers.size(), 2); // delete x2 when removing offscreensampleras
        vulkan->createDescriptorSets(descriptorSet, descriptorSetLayout, descriptorPool, vulkanUniformBuffers, samplers);

        indicesSize = indices.size();
    }

    VulkanRenderer::~VulkanRenderer()
    {
        vkDeviceWaitIdle(vulkan->device);

        vkDestroyDescriptorSetLayout(vulkan->device, descriptorSetLayout, nullptr);
        
        vkDestroyDescriptorPool(vulkan->device, descriptorPool, nullptr);

        vkDestroyBuffer(vulkan->device, vertexBuffer, nullptr);
        vkFreeMemory(vulkan->device, vertexBufferMemory, nullptr);

        vkDestroyBuffer(vulkan->device, instanceBuffer, nullptr);
        vkFreeMemory(vulkan->device, instanceBufferMemory, nullptr);

        vkDestroyBuffer(vulkan->device, indexBuffer, nullptr);
        vkFreeMemory(vulkan->device, indexBufferMemory, nullptr);

        for (auto buffer : vulkanUniformBuffers)
        {
            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                vkDestroyBuffer(Vulkan::vulkan->device, buffer.uniformBuffers[i], nullptr);
                vkFreeMemory(Vulkan::vulkan->device, buffer.uniformBuffersMemory[i], nullptr);
            }
        }
    }

    void VulkanRenderer::Render(const PipelineConfig& config, std::vector<UniformBufferObject*> uniformBuffers)
    {
        std::vector<DefaultInstance> instances;

        for (UniformBufferObject* buffer : uniformBuffers)
        {
            if (buffer->getType() == "matrix")
            {
                MatrixBufferObject* mBuffer = (MatrixBufferObject*)buffer;
                instances.emplace_back(DefaultInstance{ mBuffer->GetData().model, 0 });
            }
        }

        if (instances.empty()) instances.push_back(DefaultInstance());

        BatchRender(config, instances.data(), 1, sizeof(DefaultInstance), uniformBuffers);
    }

    void VulkanRenderer::BatchRender(const PipelineConfig& config, const void* instanceData, size_t instanceCount, size_t instanceStride, std::vector<UniformBufferObject*> uniformBuffers)
    {
        _framebuffer = vulkan->currentFramebuffer;

        for (size_t i = 0; i < uniformBuffers.size(); i++)
        {
            VulkanUniformBuffer& vulkanBuffer = vulkanUniformBuffers[i];

            size_t size = uniformBuffers[i]->SizeOf();
            vulkanBuffer.data.resize(size);
            memcpy(vulkanBuffer.data.data(), uniformBuffers[i]->Data(), size);
        }

        unsigned int pipelineId = vulkan->getOrCreatePipeline(this, config, _framebuffer != nullptr);

        std::vector<RenderData>& objectVector = config.translucent ? translucentSwapchainObjects : opaqueSwapchainObjects;
        std::vector<RenderData>& framebufferObjectVector = config.translucent ? _framebuffer->translucentObjects : _framebuffer->opaqueObjects;

        std::vector<RenderData>& vector = _framebuffer == nullptr ? objectVector : framebufferObjectVector;

        std::array<VkImageView, 5> imageViews;
        imageViews[0] = vulkan->colorTextureImageView;
        imageViews[1] = vulkan->metallicRoughnessTextureImageView;
        imageViews[2] = vulkan->emissiveTextureImageView;
        imageViews[3] = vulkan->occulsionTextureImageView;
        imageViews[4] = vulkan->normalTextureImageView;

        bool found = false;
        for (RenderData& data : vector)
        {
            if (data.renderer == this && data.pipelineId == pipelineId)
            {
                data.instanceData.Append(instanceData, instanceCount, instanceStride);
                found = true;
                break;
            }
        }

        CheckTextureChange(imageViews);

        if (!found) vector.emplace_back(RenderData{ pipelineId, this, _framebuffer, imageViews, InstanceData{ instanceData, instanceCount, instanceStride } });
    }


    void VulkanRenderer::Draw(VkCommandBuffer& commandBuffer, uint32_t currentFrame, RenderData data)
    {
        if (data.instanceData.count <= 0) return;

        size_t requiredSize = data.instanceData.count * data.instanceData.stride;
        if (requiredSize > _instanceBufferCapasity)
        {
            if (instanceBuffer != VK_NULL_HANDLE && instanceBufferMemory != VK_NULL_HANDLE)
            {
                vkDestroyBuffer(vulkan->device, instanceBuffer, nullptr);
                vkFreeMemory(vulkan->device, instanceBufferMemory, nullptr);
            }

            _instanceBufferCapasity = requiredSize * 2;
            vulkan->createInstanceBuffer(instanceBuffer, instanceBufferMemory, _instanceBufferCapasity);
        }

        void* memory = nullptr;
        VkResult result = vkMapMemory(vulkan->device, instanceBufferMemory, 0, requiredSize, 0, &memory);

        if (result == VK_SUCCESS && memory != nullptr)
        {
            memcpy(memory, data.instanceData.Data(), requiredSize);
            vkUnmapMemory(vulkan->device, instanceBufferMemory);
        }
        else
        {
            Log::Error("Failed to map instance buffer memory!");
            return;
        }

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan->GetPipeline(data.pipelineId));

        const VkBuffer buffers[2] = { vertexBuffer, instanceBuffer };
        constexpr VkDeviceSize offs[2] = { 0, 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 2, buffers, offs);
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        UpdateUniformBuffer(currentFrame, 0);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan->GetPipelineLayout(data.pipelineId), 0, 1, &descriptorSet[currentFrame], 0, nullptr);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesSize), data.instanceData.count, 0, 0, 0);
    }

    void VulkanRenderer::CheckTextureChange(std::array<VkImageView, 5> imageViews)
    {
        vulkan->UpdateTexture(descriptorSet, lastTextureColor, imageViews[0],
        vulkanUniformBuffers.size());

        vulkan->UpdateTexture(descriptorSet, lastTextureMetallicRoughness, imageViews[1],
            vulkanUniformBuffers.size() + 1);
        vulkan->UpdateTexture(descriptorSet, lastTextureEmission, imageViews[2],
            vulkanUniformBuffers.size() + 2);
        vulkan->UpdateTexture(descriptorSet, lastTextureOcculsion, imageViews[3],
            vulkanUniformBuffers.size() + 3);
        vulkan->UpdateTexture(descriptorSet, lastTextureNormal, imageViews[4],
            vulkanUniformBuffers.size() + 4);
    }

    void VulkanRenderer::UpdateUniformBuffer(uint32_t currentImage, unsigned int usedIndex)
    {
        for (auto& buffer : vulkanUniformBuffers)
        {
            if (!buffer.uniformBufferObject) {
                VGF::Log::Error("UniformBufferObject pointer not set!");
            }

            memcpy(buffer.uniformBuffersMapped[currentImage], buffer.data.data(), buffer.data.size());
        }
    }
}