#pragma once

#include "kve_window.hpp"
#include "kve_device.hpp"
#include "kve_swap_chain.hpp"

// std
#include <memory>
#include <vector>
#include <cassert>

namespace kve {
	class KveRenderer {

	public:
		KveRenderer(KveWindow& window, KveDevice& device);
		~KveRenderer();

		VkRenderPass getSwapChainRenderPass() const { return kveSwapChain->getRenderPass(); }
		float getAspectRatio() const { return kveSwapChain->extentAspectRatio(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		KveRenderer(const KveRenderer&) = delete;
		KveRenderer& operator=(const KveRenderer&) = delete;

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		KveWindow& kveWindow;
		KveDevice& kveDevice;
		std::unique_ptr<KveSwapChain> kveSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{ false };
	};
}