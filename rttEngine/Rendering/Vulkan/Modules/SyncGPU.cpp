#include "SyncGPU.h"

namespace rttvk {
	Semaphore::Semaphore(LogicalDevice* device)
	{
		this->device = device;
	}
	void Semaphore::Create()
	{
		VkSemaphoreCreateInfo createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VK_CREATE_VALIDATION(vkCreateSemaphore(device->GetDevice(), &createInfo, nullptr, &semaphore),VkSemaphore);
	}
	void Semaphore::Destroy()
	{
		vkDestroySemaphore(device->GetDevice(), semaphore, nullptr);
	}
	VkSemaphore Semaphore::GetSemaphore()
	{
		return semaphore;
	}
	Fence::Fence(LogicalDevice* device)
	{
		this->device = device;
	}
	void Fence::Create()
	{
		VkFenceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		VK_CREATE_VALIDATION(vkCreateFence(device->GetDevice(), &createInfo, nullptr, &fence), VkFence);
	}
	void Fence::Destroy()
	{
		vkDestroyFence(device->GetDevice(), fence, nullptr);
	}
	VkFence Fence::GetFence()
	{
		return fence;
	}
}

