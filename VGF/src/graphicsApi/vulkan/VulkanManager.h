#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.hpp>
#include <stb_image.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <optional>
#include <set>
#include <chrono>

#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>


#include "../../PipelineConfig.h"
#include "../../UniformBuffers/UniformBuffer.h"

#include "../../RenderImpl.h"
#include "../../Window.h"

#include "../../Log.h"

#include "VulkanGraphicsPipeline.h"
#include "VulkanFrameBuffer.h"
#include "VulkanGui.h"

namespace VGF::Vulkan
{
    inline static const int MAX_FRAMES_IN_FLIGHT = 2;

    struct InstanceData
    {
        std::vector<std::byte> data;
        size_t count;
        size_t stride;

        InstanceData(const void* data, size_t count, size_t stride)
            : data(static_cast<const std::byte*>(data), static_cast<const std::byte*>(data) + count * stride)
            , count(count)
            , stride(stride)
        { }

        void Append(const void* data, size_t count, size_t stride)
        {
            this->stride = stride;

            const std::byte* src = static_cast<const std::byte*>(data);
            this->data.insert(this->data.end(), src, src + count * stride);
            count += count;
        }

        const void* Data() { return data.data(); }
    };

    struct RenderData
    {
        unsigned int pipelineId;
        VulkanRenderer* renderer;
        VulkanFrameBuffer* framebuffer;

        std::array<VkImageView, 5> imageViews;

        InstanceData instanceData;
    };

    struct PipelineData
    {
        VkPipeline pipeline;
        VkPipelineLayout pipelineLayout;
        unsigned int index = 0;
        bool offscreen = false;
    };


    struct VulkanUniformBuffer
    {
        UniformBufferObject* uniformBufferObject;
        std::vector<unsigned int> data;
        std::vector<VkBuffer> uniformBuffers;
        std::vector<void*> uniformBuffersMapped;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
    };

    struct VulkanImageSampler
    {
        std::vector<VkImageView> imageViews;
        VkSampler& textureSampler;

        VulkanImageSampler(const std::vector<VkImageView>& views, VkSampler& sampler) 
            : imageViews(views), textureSampler(sampler) {}
    };

    class VulkanRenderer;

    class VulkanTexture : public TextureImpl
    {
    public:
        VulkanTexture(std::string filePath);
        VulkanTexture(const unsigned char* data, int format, unsigned int width, unsigned int height);

        ImTextureID GetImGuiTexture() override;

        ~VulkanTexture();
        void Bind(textureType type) override;
        void* GetNativeImage() override;
    private:
        ImTextureID textureId = NULL;

        VkImage textureImage = VK_NULL_HANDLE;
        VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;
        VkImageView textureImageViewTex = VK_NULL_HANDLE;
    };

    class Vulkan : public ApiImpl
    {
    public:
        Vulkan(GLFWwindow* GLFW_Window);
        ~Vulkan();

        void renderEnd();
        void render() override;

        static Vulkan* vulkan;

        bool vulkanGui = false;

        VkInstance instance;
        
        VkDevice device;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkFormat swapChainImageFormat;

        VkQueue graphicsQueue;

        std::vector<VulkanFrameBuffer*> framebuffers;
        std::vector<VulkanFrameBuffer*> allFramebuffers;

        VkImageView colorTextureImageView = VK_NULL_HANDLE;
        VkImageView metallicRoughnessTextureImageView = VK_NULL_HANDLE;
        VkImageView emissiveTextureImageView = VK_NULL_HANDLE;
        VkImageView occulsionTextureImageView = VK_NULL_HANDLE;
        VkImageView normalTextureImageView = VK_NULL_HANDLE;

        std::vector<VkCommandBuffer> commandBuffers;

        VkImageView depthImageView;

        VkSampler textureSampler;
        std::vector<VkImageView> offscreenImageViews;
        std::array<VkImageView, MAX_FRAMES_IN_FLIGHT + 1> lastTextures = {VK_NULL_HANDLE};


        static inline std::unordered_map<PipelineHashKey, PipelineData, PipelineConfigKeyHash> pipelineCache = {};
        static inline std::vector<std::pair<VkPipeline, VkPipelineLayout>> pipelines = {};

        unsigned int getOrCreatePipeline(VulkanRenderer* object, const PipelineConfig& config, const bool offscreen);
        PipelineData createGraphicsPipeline(VulkanRenderer* object, const PipelineConfig& config, VkRenderPass& renderPass);

        VkPipeline& GetPipeline(unsigned int pipelineID);
        VkPipelineLayout& GetPipelineLayout(unsigned int pipelineID);

        VkFormat findDepthFormat() const;

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        
        void createImages(std::vector<VkImage>& images, std::vector<VkDeviceMemory>& imageMemory);
        void createImageViews(std::vector<VkImageView>& imageViews, const std::vector<VkImage> images);
        void createFramebuffers(std::vector<VkFramebuffer>& framebuffers, const std::vector<VkImageView> imageViews, bool offscreen);

        VulkanFrameBuffer* currentFramebuffer = VK_NULL_HANDLE;

        void UpdateTexture(const std::vector<VkDescriptorSet> descriptorSets, VkImageView& lastTexture, const VkImageView imageView, const uint32_t binding);

        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

        VkCommandBuffer beginSingleTimeCommands() const;
        void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;

        void createVertexBuffer(VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory, std::vector<GLfloat>& vertices);
        void createInstanceBuffer(VkBuffer& instanceBuffer, VkDeviceMemory& instanceBufferMemory, VkDeviceSize size);

        void createIndexBuffer(VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory, std::vector<uint32_t> indices);

        void createUniformBuffers
        (
            VulkanUniformBuffer& buffer,
            size_t typeSize
        );

        void createDescriptorSetLayout(VkDescriptorSetLayout& descriptorsetLayout, std::vector<VulkanUniformBuffer>& uniformBuffers, std::vector<VulkanImageSampler>& imageSamplers);
        VkDescriptorPool createDescriptorPool(VkDescriptorPool& descriptorPool, size_t uniformBufferCount, size_t imageSamplerCount, unsigned int maxSets);
        void createDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, VkDescriptorSetLayout layout, VkDescriptorPool descriptorPool, std::vector<VulkanUniformBuffer>& uniformBuffers, std::vector<VulkanImageSampler>& imageSamplers);

        uint32_t getCurrentFrame() { return currentFrame; }

        VkRenderPass renderPass;
        VkRenderPass offscreenRenderPass;

    private:
        GLFWwindow* window;

        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR surface;

        VkQueue presentQueue;

        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkFramebuffer> swapChainFramebuffers;

        
        std::vector<VkImage> offscreenImages;
        std::vector <VkDeviceMemory> offscreenImageMemory;
        VkFormat offscreenImageFormat;
        std::vector<VkFramebuffer> offscreenFramebuffers;

        VkCommandPool commandPool;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;

        std::vector<VkCommandBuffer> offscreenCommandBuffers;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        uint32_t currentFrame = 0;

        bool framebufferResized = false;


        void createInstance();
        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
        
        struct QueueFamilyIndices;
        void pickPhysicalDevice();
        bool isDeviceSuitable(VkPhysicalDevice device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        void createSurface(GLFWwindow* window);

        struct SwapChainSupportDetails;
        void createSwapChain();
        void cleanupSwapChain();
        void recreateSwapChain();
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
        
        void createTextureSampler();

        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
        void createCommandPool();
        void createCommandBuffers();

        void createDepthResources();
        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
        bool hasStencilComponent(VkFormat format);

        void drawFrame();
        void createSyncObjects();

        void createRenderPass();
        void createOffscreenRenderPass();
        void createLogicalDevice();

        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void setupDebugMessenger();
        std::vector<const char*> getRequiredExtensions();
        bool checkValidationLayerSupport();
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

    };

    class VulkanRenderer : public RendererImpl
    {
    public:
        int indicesSize = 0;

        VulkanRenderer(std::vector<GLuint>& indices, std::vector<GLfloat>& vertices, std::vector<UniformBufferObject*> uniformBuffers);
        ~VulkanRenderer();

        VkBuffer indexBuffer = VK_NULL_HANDLE;
        VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

        VkBuffer vertexBuffer = VK_NULL_HANDLE;
        VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;

        VkBuffer instanceBuffer = VK_NULL_HANDLE;
        VkDeviceMemory instanceBufferMemory = VK_NULL_HANDLE;

        std::vector<VulkanUniformBuffer> vulkanUniformBuffers;

        VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        std::vector<VkDescriptorSet> descriptorSet;
        
        virtual void Render(const PipelineConfig& config, std::vector<UniformBufferObject*> uniformBuffers) override;
        virtual void BatchRender(const PipelineConfig& config, const void* instanceData, size_t instanceCount, size_t instanceStride, std::vector<UniformBufferObject*> uniformBuffers) override;

        void Draw(VkCommandBuffer& commandBuffer, uint32_t currentFrame, RenderData data);
    private:
        void CheckTextureChange(std::array<VkImageView, 5> imageViews);
        void UpdateUniformBuffer(uint32_t currentImage, unsigned int usedIndex);

        size_t _instanceBufferCapasity = 0;

        VulkanFrameBuffer* _framebuffer = VK_NULL_HANDLE;

        VkImageView lastTextureColor = VK_NULL_HANDLE;
        VkImageView lastTextureMetallicRoughness = VK_NULL_HANDLE;
        VkImageView lastTextureEmission = VK_NULL_HANDLE;
        VkImageView lastTextureOcculsion = VK_NULL_HANDLE;
        VkImageView lastTextureNormal = VK_NULL_HANDLE;

        Vulkan* vulkan = nullptr;
    };
}
    