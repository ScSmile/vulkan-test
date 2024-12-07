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
	class PointLightSystem
	{
	public:
		PointLightSystem(SeDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void update(FrameInfo& frameInfo, GlobalUbo& ubo);
		void render(FrameInfo& info);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		SeDevice& seDevice;
		std::unique_ptr<SePipeline> sePipeline;
		VkPipelineLayout pipelineLayout;
	};
}