#pragma once

#include "kve_camera.hpp"
#include "kve_game_object.hpp"

#include <vulkan/vulkan.h>

namespace kve {
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		KveCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		KveGameObject::Map& gameObjects;
	};
}