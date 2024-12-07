#pragma once

#include "se_device.hpp"
#include "se_game_object.hpp"
#include "se_renderer.hpp"
#include "se_window.hpp"
#include "se_descriptors.hpp"

#include <memory>
#include <vector>

namespace se 
{
	class FirstApp
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;

		void run();

	private:
		void loadGameObjects();

		SeWindow seWindow{ WIDTH, HEIGHT, "Hello, sea++" };
		SeDevice seDevice{ seWindow };
		SeRenderer seRenderer{ seWindow, seDevice };

		std::unique_ptr<SeDescriptorPool> globalPool{};
		SeGameObject::Map gameObjects;
	};
} 