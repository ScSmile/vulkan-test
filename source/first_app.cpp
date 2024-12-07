#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "se_buffer.hpp"
#include "se_camera.hpp"
#include "systems//point_light_system.hpp"
#include "systems//simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <chrono>
#include <cassert>
#include <numeric>
#include <stdexcept>

namespace se
{
	FirstApp::FirstApp()
	{
		globalPool = SeDescriptorPool::Builder(seDevice)
			.setMaxSets(SeSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SeSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		loadGameObjects();
	}

	FirstApp::~FirstApp() {}

	void FirstApp::run()
	{
		std::vector<std::unique_ptr<SeBuffer>> uboBuffers(SeSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < uboBuffers.size(); ++i)
		{
			uboBuffers[i] = std::make_unique<SeBuffer>(
				seDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = SeDescriptorSetLayout::Builder(seDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();
		
		std::vector<VkDescriptorSet> globalDescriptorSets(SeSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			SeDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ 
			seDevice, 
			seRenderer.getSwapChainRenderPass(), 
			globalSetLayout->getDescriptorSetLayout()};
		PointLightSystem pointLightSystem{
			seDevice,
			seRenderer.getSwapChainRenderPass(),
			globalSetLayout->getDescriptorSetLayout() };
		SeCamera camera{};

		auto viewerObject = SeGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
		KeyboardMovementController cameraController{};
		
		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!seWindow.shouldClose())
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.moveInPlaneXZ(seWindow.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = seRenderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(45.f), aspect, 0.1f, 10.f);

			if (auto commandBuffer = seRenderer.beginFrame())
			{
				int frameIndex = seRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					gameObjects
				};

				// update
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				pointLightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// render
				seRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);
				seRenderer.endSwapChainRenderPass(commandBuffer);
				seRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(seDevice.device());
	}

	

	void FirstApp::loadGameObjects()
	{
		std::shared_ptr<SeModel> seModel = 
			SeModel::createModelFromFile(seDevice, "models/flat_vase.obj");
		auto flatVase = SeGameObject::createGameObject();
		flatVase.model = seModel;
		flatVase.transform.translation = { -0.5f, .5f, 0.f };
		flatVase.transform.scale = glm::vec3{ 3.f, 1.5f, 3.f };
		gameObjects.emplace(flatVase.getID(), std::move(flatVase));

		seModel = SeModel::createModelFromFile(seDevice, "models/smooth_vase.obj");
		auto smoothVase = SeGameObject::createGameObject();
		smoothVase.model = seModel;
		smoothVase.transform.translation = { .5f, .5f, 0.f };
		smoothVase.transform.scale = glm::vec3{ 3.f, 1.5f, 3.f };
		gameObjects.emplace(smoothVase.getID(), std::move(smoothVase));

		seModel = SeModel::createModelFromFile(seDevice, "models/quad.obj");
		auto floor = SeGameObject::createGameObject();
		floor.model = seModel;
		floor.transform.translation = { 0.f, .5f, 0.f };
		floor.transform.scale = glm::vec3{ 3.f, 1.f, 3.f };
		gameObjects.emplace(floor.getID(), std::move(floor));

		std::vector<glm::vec3> lightColors{
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f}  
		};

		for (size_t i = 0; i < lightColors.size(); i++) 
		{
			auto pointLight = SeGameObject::makePointLight(0.5f, 0.05f);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(
				glm::mat4(1.f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				{ 0.f, -1.f, 0.f });

			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f));
			gameObjects.emplace(pointLight.getID(), std::move(pointLight));
		}
	}
}