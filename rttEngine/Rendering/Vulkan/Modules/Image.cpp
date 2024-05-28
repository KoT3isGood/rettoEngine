#include "Image.h"

namespace rttvk {
	Image::Image(LogicalDevice* device, uint32_t x, uint32_t y)
	{
		this->device = device;
		this->x = x;
		this->y = y;
		format = VK_FORMAT_R8G8B8A8_UNORM;
	}
	Image::Image(LogicalDevice* device, uint32_t x, uint32_t y, VkFormat format)
	{
		this->device = device;
		this->x = x;
		this->y = y;
		this->format = format;
	}
	void Image::Create()
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = x;
		imageInfo.extent.height = y;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_STORAGE_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		VK_CREATE_VALIDATION(vkCreateImage(device->GetDevice(), &imageInfo, nullptr, &textureImage));

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device->GetDevice(), textureImage, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		VK_CREATE_VALIDATION(vkAllocateMemory(device->GetDevice(), &allocInfo, nullptr, &textureImageMemory));

		vkBindImageMemory(device->GetDevice(), textureImage, textureImageMemory, 0);

		imageView = ImageView(device, textureImage, VK_FORMAT_R32G32B32A32_SFLOAT);
		imageView.Create();

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_FALSE;
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(*device->GetPhysicalDevice(), &properties);
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		VK_CREATE_VALIDATION(vkCreateSampler(device->GetDevice(), &samplerInfo, nullptr, &textureSampler));
	}

	void Image::Destroy()
	{
		imageView.Destroy();
		vkFreeMemory(device->GetDevice(), textureImageMemory, nullptr);
		vkDestroyImage(device->GetDevice(), textureImage, nullptr);
		vkDestroySampler(device->GetDevice(), textureSampler, nullptr);
	}
	uint32_t Image::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(*device->GetPhysicalDevice(), &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}
}
