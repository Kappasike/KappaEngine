#pragma once

#include "kve_device.hpp"

#include <memory>

namespace kve {
	class KveTexture {

	public:

		void createTextureImage(KveDevice& device, const std::string& filepath);
		void createTextureImageSampler(KveDevice& device);

		
		KveTexture(KveDevice& device);
		~KveTexture();

		KveTexture(const KveTexture&) = delete;
		KveTexture& operator=(const KveTexture&) = delete;

		std::unique_ptr<KveTexture> createTextureFromFile(KveDevice& device, const std::string& filepath);


	private:
		//void createImage(KveDevice& device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		
		KveDevice& kveDevice;

		VkImage textureImage;
		VkSampler textureSampler;
		VkDeviceMemory textureImageMemory;

		
		
	};
}