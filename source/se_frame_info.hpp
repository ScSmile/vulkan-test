#pragma once

#include "se_camera.hpp"
#include "se_game_object.hpp"

#include <vulkan/vulkan.h>

namespace se
{
	constexpr size_t MAX_LIGHTS = 10;

	struct PointLight
	{
		glm::vec4 position{}; // ignore w
		glm::vec4 color{}; // w is intensity
	};
	
	struct GlobalUbo
	{
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };
		PointLight pointLights[MAX_LIGHTS];
		int numLights;
	};

	struct FrameInfo
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		SeCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		SeGameObject::Map& gameObjects;
	};
}