#pragma once

#include "se_model.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

namespace se
{
	struct TransformComponent
	{
		glm::vec3 translation{}; //position offset
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		glm::vec3 rotation{};

		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	struct PointLightComponent
	{
		float lightIntensity = 1.0f;
	};

	class SeGameObject
	{
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, SeGameObject>;

		static SeGameObject createGameObject()
		{
			static id_t currentId = 0;
			return SeGameObject(currentId++);
		}

		static SeGameObject makePointLight(
			float intensity = 10.f,
			float radius = 0.1f,
			glm::vec3 color = glm::vec3(1.f));

		SeGameObject(const SeGameObject&) = delete;
		SeGameObject& operator=(const SeGameObject&) = delete;
		SeGameObject(SeGameObject&&) = default;
		SeGameObject& operator=(SeGameObject&&) = default;

		id_t getID() const { return id; }

		glm::vec3 color;
		TransformComponent transform{};

		//optional components
		std::shared_ptr<SeModel> model{};
		std::unique_ptr<PointLightComponent> pointLight = nullptr;

	private:
		SeGameObject(id_t objId) : id{ objId } {}

		id_t id;
	};
}