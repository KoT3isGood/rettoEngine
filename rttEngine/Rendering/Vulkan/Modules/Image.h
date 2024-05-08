#pragma once
#include "Module.h"
#include "LogicalDevice.h"
#include "CommandBuffer.h"
#include "Buffer.h"
#include "ImageView.h"

namespace rttvk {

	class Image : public Module {
	public:
		Image(LogicalDevice* device, uint32_t x, uint32_t y);
		virtual void Create() override;
		virtual void Destroy() override;
		ImageView imageView;
		VkSampler textureSampler;
		VkImage textureImage;
	private:
		uint32_t x;
		uint32_t y;
		LogicalDevice* device;
		std::string filepath;
		VkDeviceMemory textureImageMemory;
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	};
}