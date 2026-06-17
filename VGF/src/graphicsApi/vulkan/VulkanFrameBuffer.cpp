#include "VulkanFrameBuffer.h"
#include "VulkanManager.h"

namespace VGF::Vulkan
{

	VulkanFrameBuffer::VulkanFrameBuffer(void* imageVieww, unsigned int width, unsigned int height, bool offscreen)
	{
        extent.width = width;
        extent.height = height;
        _offscreen = offscreen;

        _vulkan = Vulkan::vulkan;
		imageView = static_cast<VkImageView>(imageVieww);

        std::array<VkImageView, 2> attachments = { imageView, _vulkan->depthImageView };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = _offscreen ? _vulkan->offscreenRenderPass : _vulkan->renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = width;
        framebufferInfo.height = height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(_vulkan->device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
            throw std::runtime_error("failed to create framebuffer!");

        _vulkan->allFramebuffers.emplace_back(this);
	}

    void VulkanFrameBuffer::Recreate()
    {
        vkDestroyFramebuffer(_vulkan->device, framebuffer, nullptr);

        std::array<VkImageView, 2> attachments = { imageView, _vulkan->depthImageView };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = _offscreen ? _vulkan->offscreenRenderPass : _vulkan->renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(_vulkan->device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
            throw std::runtime_error("failed to create framebuffer!");
    }

	VulkanFrameBuffer::~VulkanFrameBuffer()
	{
        vkDestroyFramebuffer(_vulkan->device, framebuffer, nullptr);
	}


	void VulkanFrameBuffer::Bind() { _vulkan->currentFramebuffer = this; _vulkan->framebuffers.push_back(this); }
    void VulkanFrameBuffer::UnBind() { _vulkan->currentFramebuffer = nullptr; }
	void VulkanFrameBuffer::Delete(){}
}