#pragma once

#include "se_device.hpp"
#include "se_swap_chain.hpp"
#include "se_window.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace se
{
	class SeRenderer
	{
	public:
		SeRenderer(SeWindow& window, SeDevice& device);
		~SeRenderer();

		SeRenderer(const SeRenderer&) = delete;
		SeRenderer& operator=(const SeRenderer&) = delete;


		VkRenderPass getSwapChainRenderPass() const { return seSwapChain->getRenderPass(); }
		float getAspectRatio() const { return seSwapChain->extentAspectRatio(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const
		{
			assert(isFrameStarted && "Cannot get command buffer when frame is not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const
		{
			assert(isFrameStarted && "Cannot get frame index when frame is not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		SeWindow& seWindow;
		SeDevice& seDevice;
		std::unique_ptr<SeSwapChain> seSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{ false };
	};
}