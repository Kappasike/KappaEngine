#include "kve_renderer.hpp"

#include <stdexcept>
#include <array>
#include <cassert>

namespace kve {

	KveRenderer::KveRenderer(KveWindow& window, KveDevice& device) : kveWindow{ window }, kveDevice{ device } {
		recreateSwapChain();
		createCommandBuffers();
	}

	KveRenderer::~KveRenderer() { freeCommandBuffers(); }

	void KveRenderer::recreateSwapChain() {
		auto extent = kveWindow.getExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = kveWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(kveDevice.device());
		kveSwapChain = nullptr;
		//vkDestroySwapchainKHR(kveDevice.device(), kveSwapChain->getSwapchain(), nullptr);
		kveSwapChain = std::make_unique<KveSwapChain>(kveDevice, extent);
		/*if (kveSwapChain == nullptr)
		{
			kveSwapChain = std::make_unique<KveSwapChain>(kveDevice, extent);
		}
		else
		{
			std::shared_ptr<KveSwapChain> oldSwapChain = std::move(kveSwapChain);
			kveSwapChain = std::make_unique<KveSwapChain>(kveDevice, extent, oldSwapChain);

			if(!oldSwapChain->compareSwapFormats(*kveSwapChain.get()))
			{
				throw std::runtime_error("Swap chain image (or depth) format has changed!");
			}
		}*/
	}

	VkCommandBuffer KveRenderer::beginFrame()
	{
		assert(!isFrameStarted && "Can't call beginFrame while frame already in progress");
		
		auto result = kveSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire next swap chain image");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer");
		}

		return commandBuffer;
	}

	void KveRenderer::endFrame()
	{
		assert(isFrameStarted && "Can't call endFrame while frame is not in progress");

		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer");
		}

		auto result = kveSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || kveWindow.wasWindowResized())
		{
			kveWindow.resetWindowResizedFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % KveSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void KveRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame isn't in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = kveSwapChain->getRenderPass();
		renderPassInfo.framebuffer = kveSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = kveSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.f };
		clearValues[1].depthStencil = { 1.f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.f;
		viewport.y = 0.f;
		viewport.width = static_cast<float>(kveSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(kveSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;
		VkRect2D scissor{ {0, 0}, kveSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void KveRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame isn't in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");
		vkCmdEndRenderPass(commandBuffer);
	}

	void KveRenderer::createCommandBuffers() {
		commandBuffers.resize(KveSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = kveDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(kveDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers");
		}
	}

	void KveRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(kveDevice.device(), kveDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}
}
