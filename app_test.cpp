#include "app_test.hpp"

#include "kve_input_manager.hpp"
#include "kve_simple_render_system.hpp"
#include "kve_camera.hpp"
#include "kve_buffer.hpp"

// glm libraries
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <chrono>
#include <cassert>
#include <stdexcept>

namespace kve {

	struct GlobalUbo {
		glm::mat4 projectionView{1.f};
		glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f};
		//glm::vec3 lightDirection = glm::normalize(glm::vec3(1.f, -3.f, -1.f));
		glm::vec3 lightPosition{-1.f};
		alignas(16) glm::vec4 lightColor{1.f};
	};

	AppTest::AppTest() {
		globalPool = KveDescriptorPool::Builder(kveDevice).setMaxSets(KveSwapChain::MAX_FRAMES_IN_FLIGHT).addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, KveSwapChain::MAX_FRAMES_IN_FLIGHT).build();
		loadGameObjects();
	}

	AppTest::~AppTest() {}

	void AppTest::loadGameTextures()
	{
	}

	void AppTest::run()
	{
		std::vector<std::unique_ptr<KveBuffer>> uboBuffers(KveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++)
		{
			uboBuffers[i] = std::make_unique<KveBuffer>(kveDevice, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = KveDescriptorSetLayout::Builder(kveDevice).addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS).build();

		std::vector<VkDescriptorSet> globalDescriptorSets(KveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			KveDescriptorWriter(*globalSetLayout, *globalPool).writeBuffer(0, &bufferInfo).build(globalDescriptorSets[i]);
		}
		
		KveSimpleRenderSystem kveSimpleRenderSystem{ kveDevice, kveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		KveCamera camera{};

		auto viewerObject = KveGameObject::createGameObject();
		KveInputManager cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!kveWindow.shouldClose())
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.moveInPlaneXZ(kveWindow.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = kveRenderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.001f, 100.f);
			
			if (auto commandBuffer = kveRenderer.beginFrame())
			{
				int frameIndex = kveRenderer.getFrameIndex();
				FrameInfo frameInfo{ frameIndex,frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex], gameObjects};

				// update
				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// render
				kveRenderer.beginSwapChainRenderPass(commandBuffer);
				kveSimpleRenderSystem.renderGameObjects(frameInfo);
				kveRenderer.endSwapChainRenderPass(commandBuffer);
				kveRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(kveDevice.device());
	}

	void AppTest::loadGameObjects()
	{
		std::shared_ptr<KveModel> kunaiModel = KveModel::createModelFromFile(kveDevice, "Models/kunai.obj");
		std::shared_ptr<KveModel> vaseModel = KveModel::createModelFromFile(kveDevice, "Models/smooth_vase.obj");
		std::shared_ptr<KveModel> planeModel = KveModel::createModelFromFile(kveDevice, "Models/quad.obj");

		auto kunai = KveGameObject::createGameObject();
		kunai.model = kunaiModel;
		kunai.transform.translation = { 0.f, 0.f, 2.5f };
		kunai.transform.scale = glm::vec3{ 1.f };
		gameObjects.emplace(kunai.getId(), std::move(kunai));
			
		auto vase = KveGameObject::createGameObject();
		vase.model = vaseModel;
		vase.transform.translation = { 2.f, -2.f, 0.f };
		vase.transform.scale = glm::vec3{ 5.f };
		gameObjects.emplace(vase.getId(), std::move(vase));

		auto plane = KveGameObject::createGameObject();
		plane.model = planeModel;
		plane.transform.translation = { 0.f, 0.5f, 0.f };
		plane.transform.scale = glm::vec3{ 3.f, 1.f, 3.f };
		gameObjects.emplace(plane.getId(), std::move(plane));
	}
}
