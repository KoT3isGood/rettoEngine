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
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = usage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		//bufferCreateInfo.queueFamilyIndexCount = 1;
		//uint32_t queue = device->GetGraphicsQueueIndex();
		//bufferCreateInfo.pQueueFamilyIndices = &queue;

		VK_CREATE_VALIDATION(vkCreateBuffer(device->GetDevice(), &bufferCreateInfo, nullptr, &buffer), VkBuffer);

		vkGetBufferMemoryRequirements(device->GetDevice(), buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		VK_CREATE_VALIDATION(vkAllocateMemory(device->GetDevice(), &allocInfo, nullptr, &memory), VkMemory Allocation);
		

		VK_CREATE_VALIDATION(vkMapMemory(device->GetDevice(), memory, 0, size, 0, &mapped), Mapping Memory);
		VK_CREATE_VALIDATION(vkBindBufferMemory(device->GetDevice(), buffer, memory, 0), Bind Memory);

		
	}
	void Buffer::Destroy()
	{

		vkUnmapMemory(device->GetDevice(), memory);
		vkFreeMemory(device->GetDevice(), memory, nullptr);
		vkDestroyBuffer(device->GetDevice(), buffer, nullptr);
	}
	VkBuffer Buffer::GetBuffer()
	{
		return buffer;
	}
	
	void Buffer::SetData(void* var, size_t size)
	{
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

	VkDeviceAddress Buffer::GetBufferAddress()
	{
		VkBufferDeviceAddressInfo bdai{ VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO };
		bdai.buffer = buffer;
		return vkGetBufferDeviceAddress(device->GetDevice(), &bdai);
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