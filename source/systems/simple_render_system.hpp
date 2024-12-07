#pragma once

#include "../se_camera.hpp"
#include "../se_device.hpp"
#include "../se_frame_info.hpp"
#include "../se_game_object.hpp"
#include "../se_pipeline.hpp"

#include <memory>
#include <vector>

namespace se
{
	class SimpleRenderSystem
	{
	public:
		SimpleRenderSystem(SeDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(FrameInfo& info);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		SeDevice& seDevice;
		std::unique_ptr<SePipeline> sePipeline;
		VkPipelineLayout pipelineLayout;
	};
}