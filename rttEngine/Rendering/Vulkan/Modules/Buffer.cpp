#include "Buffer.h"

namespace rttvk {
	Buffer::Buffer(LogicalDevice* device, uint32_t size,VkBufferUsageFlags usage)
	{
		this->device = device;
		this->size = size;
		this->usage = usage;
	}
	void Buffer::Create()
	{
		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = usage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 1;
		uint32_t queue = device->GetGraphicsQueueIndex();
		bufferCreateInfo.pQueueFamilyIndices = &queue;

		VK_CREATE_VALIDATION(vkCreateBuffer(device->GetDevice(), &bufferCreateInfo, nullptr, &buffer), VkBuffer);

		vkGetBufferMemoryRequirements(device->GetDevice(), buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		VK_CREATE_VALIDATION(vkAllocateMemory(device->GetDevice(), &allocInfo, nullptr, &memory), VkMemory Allocation);
		VK_CREATE_VALIDATION(vkMapMemory(device->GetDevice(), memory, 0, 8, 0, &mapped), VkMemory Mapping);

		vkBindBufferMemory(device->GetDevice(), buffer, memory, 0);
	}
	void Buffer::Destroy()
	{
		vkUnmapMemory(device->GetDevice(), memory);
		vkDestroyBuffer(device->GetDevice(), buffer, nullptr);
	}
	VkBuffer Buffer::GetBuffer()
	{
		return buffer;
	}
	
	void Buffer::SetData(void* var, size_t size)
	{
		//RTT_LOG(std::to_string((int)var));
		memcpy(mapped, &var, size);
	}

	void* Buffer::GetMapped()
	{
		return mapped;
	}

	uint32_t Buffer::GetBufferSize()
	{
		return size;
	}
	






	uint32_t Buffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
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