#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty\stb_image.h"

namespace rttvk {
	Texture::Texture(LogicalDevice* device, std::string filepath)
	{
		this->device = device;
		this->filepath = filepath;
	}
	void Texture::Create()
	{
		stbi_uc* pixels = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		if (!pixels) {
			RTT_LOG("Failed to load: " + filepath);
			return;
		}
		VkDeviceSize imageSize = width * height * 4;
		imageBuffer = Buffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		imageBuffer.Create();
		memcpy(imageBuffer.GetMapped(), pixels, imageSize);
		stbi_image_free(pixels);

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		VK_CREATE_VALIDATION(vkCreateImage(device->GetDevice(), &imageInfo, nullptr, &textureImage));

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device->GetDevice(), textureImage, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		VK_CREATE_VALIDATION(vkAllocateMemory(device->GetDevice(), &allocInfo, nullptr, &textureImageMemory));

		vkBindImageMemory(device->GetDevice(), textureImage, textureImageMemory, 0);

		imageView = ImageView(device, textureImage);
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

		CommandPool cmdPool = CommandPool(device);
		cmdPool.Create();
		CommandBuffer cmd = CommandBuffer(device, &cmdPool);
		cmd.Create();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		

		VkImageSubresourceRange access;
		access.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		access.baseMipLevel = 0;
		access.levelCount = 1;
		access.baseArrayLayer = 0;
		access.layerCount = 1;

		VkImageMemoryBarrier barrier;
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.srcAccessMask = VK_ACCESS_NONE_KHR;
		barrier.dstAccessMask = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.image = textureImage;
		barrier.subresourceRange = access;
		barrier.pNext = nullptr;

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			(uint32_t)width,
			(uint32_t)height,
			1
		};

		vkBeginCommandBuffer(cmd.GetBuffer(), &beginInfo);

		vkCmdCopyBufferToImage(
			cmd.GetBuffer(),
			imageBuffer.GetBuffer(),
			textureImage,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);

		vkCmdPipelineBarrier(cmd.GetBuffer(), VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		vkEndCommandBuffer(cmd.GetBuffer());

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = cmd.GetBufferP();

		vkQueueSubmit(device->GetGraphicsQueue(), 1, &submitInfo, nullptr);
		vkQueueWaitIdle(device->GetGraphicsQueue());

		cmd.Destroy();
		cmdPool.Destroy();

		
		
	}
	void Texture::Destroy()
	{
		imageBuffer.Destroy();
		imageView.Destroy();
		vkFreeMemory(device->GetDevice(), textureImageMemory, nullptr);
		vkDestroyImage(device->GetDevice(), textureImage, nullptr);
		vkDestroySampler(device->GetDevice(), textureSampler, nullptr);
	}
	uint32_t Texture::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
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