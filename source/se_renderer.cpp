#include "se_renderer.hpp"

#include <array>
#include <stdexcept>

namespace se
{
	SeRenderer::SeRenderer(SeWindow& window, SeDevice& device)
		:seWindow{ window }, seDevice{ device }
	{
		recreateSwapChain();
		createCommandBuffers();
	}

	SeRenderer::~SeRenderer()
	{
		freeCommandBuffers();
	}

	void SeRenderer::recreateSwapChain()
	{
		auto extent = seWindow.getExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = seWindow.getExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(seDevice.device());

		if (seSwapChain == nullptr)
		{
			seSwapChain = std::make_unique<SeSwapChain>(seDevice, extent);
		}
		else
		{
			std::shared_ptr<SeSwapChain> oldSwapChain = std::move(seSwapChain);
			seSwapChain = std::make_unique<SeSwapChain>(seDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*seSwapChain.get()))
			{
				throw std::runtime_error("Swap chain image or depth format was changed");
			}
		}
	}

	void SeRenderer::createCommandBuffers()
	{
		commandBuffers.resize(SeSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = seDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(seDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers");
		}
	}

	void SeRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			seDevice.device(),
			seDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();
	}

	VkCommandBuffer SeRenderer::beginFrame()
	{
		assert(!isFrameStarted && "Cannot call beginFrame while already in progress");

		auto result = seSwapChain->acquireNextImage(&currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		return commandBuffer;
	}

	void SeRenderer::endFrame()
	{
		assert(isFrameStarted && "Cannot call endFrame if frame is not in progress");

		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer");
		}

		auto result = seSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR || seWindow.wasWindowResized())
		{
			seWindow.resetWindowResizedFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) %  SeSwapChain::MAX_FRAMES_IN_FLIGHT; //++currentFrameIndex %= MAX_FRAMES_IN_FLIGHT
	}

	void SeRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Cannot call beginSwapChainRenderPass if frame is not in progress");
		assert(
			commandBuffer == getCurrentCommandBuffer() && 
			"Cannot begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = seSwapChain->getRenderPass();
		renderPassInfo.framebuffer = seSwapChain->getFrameBuffer(static_cast<int>(currentImageIndex));

		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = seSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(seSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(seSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0,0}, seSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void SeRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Cannot call endSwapChainRenderPass if frame is not in progress");
		assert(
			commandBuffer == getCurrentCommandBuffer() &&
			"Cannot end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}
}