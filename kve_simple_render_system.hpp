#pragma once

#include "kve_pipeline.hpp"
#include "kve_device.hpp"
#include "kve_game_object.hpp"
#include "kve_camera.hpp"
#include "kve_frame_info.hpp"

#include <memory>
#include <vector>

namespace kve {
	class KveSimpleRenderSystem {

	public:

		KveSimpleRenderSystem(KveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~KveSimpleRenderSystem();

		KveSimpleRenderSystem(const KveSimpleRenderSystem&) = delete;
		KveSimpleRenderSystem& operator=(const KveSimpleRenderSystem&) = delete;

		void renderGameObjects(FrameInfo& frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		KveDevice& kveDevice;

		std::unique_ptr<KvePipeline> kvePipeline;
		VkPipelineLayout pipelineLayout;
	};
}