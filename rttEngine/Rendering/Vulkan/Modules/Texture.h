#pragma once
#include "Module.h"
#include "LogicalDevice.h"
#include "CommandBuffer.h"
#include "Buffer.h"
#include "ImageView.h"


namespace rttvk {

	class Texture : public Module {
	public:
		Texture(LogicalDevice* device, std::string filepath);
		virtual void Create() override;
		virtual void Destroy() override;
		ImageView imageView;
		VkSampler textureSampler;
		VkImage textureImage;
	private:
		int width, height, channels;
		LogicalDevice* device;
		std::string filepath;
		Buffer imageBuffer;
		VkDeviceMemory textureImageMemory;
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	};
}