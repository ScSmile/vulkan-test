#pragma once

#include "se_device.hpp"

#include "se_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace se
{
	class SeModel
	{
	public:

		struct Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& other) const
			{
				return position == other.position &&
					color == other.color &&
					normal == other.normal &&
					uv == other.uv;
			}
		};

		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filePath);
		};

		SeModel(SeDevice& device, const SeModel::Builder& builder);
		~SeModel();

		SeModel(const SeModel&) = delete;
		SeModel& operator=(const SeModel&) = delete;
		SeModel(SeModel&&) = default;
		SeModel& operator=(SeModel&&) = default;

		static std::unique_ptr<SeModel> createModelFromFile(
			SeDevice& device, const std::string& filePath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

		SeDevice& seDevice;

		std::unique_ptr<SeBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		std::unique_ptr<SeBuffer> indexBuffer;
		uint32_t indexCount;
	};
}