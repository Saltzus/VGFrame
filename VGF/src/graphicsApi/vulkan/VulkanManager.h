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

#include "VulkanGraphicsPipeline.h"



namespace VGF::Vulkan
{
    inline static const int MAX_FRAMES_IN_FLIGHT = 2;


    struct VulkanUniformBuffer
    {
        UniformBufferObject* uniformBufferObject;
        std::vector<VkBuffer> uniformBuffers;
        std::vector<void*> uniformBuffersMapped;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
    };

    struct VulkanImageSampler
    {
        std::vector<VkImageView> imageViews;
        VkSampler& textureSampler;
    };

    class VulkanRenderer;
    class VulkanPostProcess;

    class VulkanTexture : public TextureImpl
    {
    public:
        VulkanTexture(const char* filePath);
        VulkanTexture(const unsigned char* data, int format, int width, int height);
        ~VulkanTexture();
        void Bind(textureType type) override;
    private:
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageViewTex;
    };

    class Vulkan : public ApiImpl
    {
    public:
        Vulkan(GLFWwindow* GLFW_Window);
        ~Vulkan();

        void renderEnd();
        void render() override;

        static Vulkan* vulkan;

        VkDevice device;

        VulkanPostProcess* defaultPostProcess;

        VkImageView colorTextureImageView;
        VkImageView metallicRoughnessTextureImageView;
        VkImageView emissiveTextureImageView;
        VkImageView occulsionTextureImageView;
        VkImageView normalTextureImageView;

        VkSampler textureSampler;
        std::vector<VkImageView> offscreenImageViews;
        std::array<VkImageView, MAX_FRAMES_IN_FLIGHT + 1> lastTextures = {nullptr};

        static std::unordered_map<PipelineConfig, std::pair<VkPipeline, VkPipelineLayout>, PipelineConfigHash> pipelineCache;

        std::pair<VkPipeline, VkPipelineLayout> getOrCreatePipeline(VulkanRenderer* object, const PipelineConfig& config);
        std::pair<VkPipeline, VkPipelineLayout> getOrCreatePipeline(VulkanPostProcess* process, const PipelineConfig& config);
        std::pair<VkPipeline, VkPipelineLayout> createGraphicsPipeline(VulkanRenderer* object, const PipelineConfig& config, VkRenderPass& renderPass);
        std::pair<VkPipeline, VkPipelineLayout> createGraphicsPipeline(VulkanPostProcess* process, const PipelineConfig& config, VkRenderPass& renderPass);


        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        
        void UpdateTexture(const std::vector<VkDescriptorSet> descriptorSets, VkImageView& lastTexture, const VkImageView imageView, const uint32_t binding);
        void createPresentImages();

        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);


        std::pair<VkBuffer, VkDeviceMemory> createVertexBuffer(std::vector<GLfloat>& vertices);
        std::pair<VkBuffer, VkDeviceMemory> createIndexBuffer(std::vector<uint16_t> indices);

        void createUniformBuffers
        (
            VulkanUniformBuffer& buffer,
            size_t typeSize
        );

        void updateUniformBuffer(uint32_t currentImage, VulkanRenderer* object);

        void createDescriptorSetLayout(VkDescriptorSetLayout& descriptorsetLayout, std::vector<VulkanUniformBuffer>& uniformBuffers, std::vector<VulkanImageSampler>& imageSamplers);
        VkDescriptorPool createDescriptorPool(size_t uniformBufferCount, size_t imageSamplerCount);
        std::vector<VkDescriptorSet> createDescriptorSets(VkDescriptorSetLayout layout, std::vector<VulkanUniformBuffer>& uniformBuffers, std::vector<VulkanImageSampler>& imageSamplers);

        uint32_t getCurrentFrame() { return currentFrame; }

    private:
        GLFWwindow* window;

        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR surface;

        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        VkQueue graphicsQueue;
        VkQueue presentQueue;

        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkFramebuffer> swapChainFramebuffers;

        
        std::vector<VkImage> offscreenImages;
        std::vector <VkDeviceMemory> offscreenImageMemory;
        VkFormat offscreenImageFormat;
        std::vector<VkFramebuffer> offscreenFramebuffers;

        VkRenderPass renderPass;
        VkRenderPass offscreenRenderPass;
        //VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;


        VkCommandPool commandPool;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;

        VkImage textureImage;
        VkDeviceMemory textureImageMemory;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;



        std::vector<VkCommandBuffer> commandBuffers;
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
        
        void createImageViews();
        void createFramebuffers();
        void createTextureImageView();
        void createTextureSampler();


        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);

        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        VkShaderModule createShaderModule(const std::vector<char>& code);

        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
        void recordOffscreenCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
        void createCommandPool();
        void createCommandBuffers();

        void createDepthResources();
        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        VkFormat findDepthFormat();
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

    class VulkanPostProcess
    {
    public:
        VulkanPostProcess(std::vector<UniformBufferObject*> uniformBuffers = {});
        ~VulkanPostProcess();

        void Render(PipelineConfig& config, std::vector<UniformBufferObject*> uniformBuffers = {});

        int indicesSize = 0;


        std::pair<VkBuffer, VkDeviceMemory> vertexBuffer_vertexBufferMemory;
        std::pair<VkBuffer, VkDeviceMemory> indexBuffer_indexBufferMemory;

        std::vector<VulkanUniformBuffer> vulkanUniformBuffers;
        VkDescriptorSetLayout descriptorSetLayout;
        PipelineConfig config;

        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;

    private:
        Vulkan* vulkan = nullptr;
    };

    class VulkanRenderer : public RendererImpl
    {
    public:
        //unsigned int id;
        int indicesSize = 0;

        VulkanRenderer(std::vector<GLuint>& indices, std::vector<GLfloat>& vertices, std::vector<UniformBufferObject*> uniformBuffers);
        ~VulkanRenderer();

        PipelineConfig config;

        std::pair<VkBuffer, VkDeviceMemory> vertexBuffer_vertexBufferMemory;
        std::pair<VkBuffer, VkDeviceMemory> indexBuffer_indexBufferMemory;

        std::vector<VulkanUniformBuffer> vulkanUniformBuffers;

        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;

        virtual void Render(PipelineConfig& config, std::vector<UniformBufferObject*> uniformBuffers) override;
    private:
        void CheckTextureChange();
        
        VkImageView lastTextureColor = nullptr;
        VkImageView lastTextureMetallicRoughness = nullptr;
        VkImageView lastTextureEmission = nullptr;
        VkImageView lastTextureOcculsion = nullptr;
        VkImageView lastTextureNormal = nullptr;


        Vulkan* vulkan = nullptr;
    };
}
    