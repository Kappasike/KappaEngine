#pragma once

#include "kve_device.hpp"
#include "kve_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace kve {
	class KveModel {

	public:

		struct Vertex {
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& other) const {
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		struct Data {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filepath);
		};

		KveModel(KveDevice& device, const KveModel::Data& data);
		~KveModel();

		KveModel(const KveModel&) = delete;
		KveModel& operator=(const KveModel&) = delete;

		static std::unique_ptr<KveModel> createModelFromFile(KveDevice& device, const std::string& filepath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

		KveDevice& kveDevice;
	
		std::unique_ptr<KveBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer{false};
		std::unique_ptr<KveBuffer> indexBuffer;
		uint32_t indexCount;
	};
}