#pragma once
#include "Module.h"
#include "LogicalDevice.h"

namespace rttvk {
	class Buffer: public Module {
	public:
		Buffer() = default;
		Buffer(LogicalDevice* device, uint32_t size, VkBufferUsageFlags usage);
		virtual void Create() override;
		virtual void Destroy() override;
		VkBuffer GetBuffer();
		void SetData(void* var, size_t size);
		void* GetMapped();
		uint32_t GetBufferSize();

		VkDeviceAddress GetBufferAddress();
	private:
		LogicalDevice* device;
		VkDeviceMemory memory;
		VkBuffer buffer;
		VkMemoryRequirements memRequirements;
		void* mapped;
		uint32_t size;
		VkBufferUsageFlags usage;

		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;
	};

}
