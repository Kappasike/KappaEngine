#pragma once

#include "kve_window.hpp"
#include "kve_device.hpp"
#include "kve_renderer.hpp"
#include "kve_game_object.hpp"
#include "kve_descriptors.hpp"

#include <memory>
#include <vector>

namespace kve {
	class AppTest {

	public:
		static constexpr int WIDTH = 1280;
		static constexpr int HEIGHT = 720;

		AppTest();
		~AppTest();
		
		AppTest(const AppTest&) = delete;
		AppTest& operator=(const AppTest&) = delete;

		void run();
		
	private:
		void loadGameTextures();
		void loadGameObjects();

		KveWindow kveWindow{ WIDTH, HEIGHT, "Kappa's World" };
		KveDevice kveDevice{ kveWindow };
		KveRenderer kveRenderer{ kveWindow, kveDevice };

		// order of declaration matter
		std::unique_ptr<KveDescriptorPool> globalPool{};
		KveGameObject::Map gameObjects;
	};
}