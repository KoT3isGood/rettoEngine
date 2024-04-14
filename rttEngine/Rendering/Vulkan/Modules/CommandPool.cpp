#include "CommandPool.h"
namespace rttvk {
	CommandPool::CommandPool(LogicalDevice* device)
	{
		this->device = device;
	}
	void CommandPool::Create()
	{
		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		createInfo.queueFamilyIndex = device->GetGraphicsQueueIndex();

		VK_CREATE_VALIDATION(vkCreateCommandPool(device->GetDevice(), &createInfo, nullptr, &pool));
	}
	void CommandPool::Destroy()
	{
		vkDestroyCommandPool(device->GetDevice(), pool, nullptr);
	}
	VkCommandPool CommandPool::GetPool()
	{
		return pool;
	}
}