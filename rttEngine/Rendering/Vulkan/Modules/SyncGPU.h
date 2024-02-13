#pragma once
#include "Module.h"
#include "LogicalDevice.h"
namespace rttvk {
	class Semaphore : public Module {
	public:
		Semaphore(LogicalDevice* device);
		virtual void Create() override;
		virtual void Destroy() override;
		VkSemaphore GetSemaphore();
	private:
		LogicalDevice* device;
		VkSemaphore semaphore;
	};
	class Fence : public Module {
	public:
		Fence(LogicalDevice* device);
		virtual void Create() override;
		virtual void Destroy() override;
		VkFence GetFence();
	private:
		LogicalDevice* device;
		VkFence fence;
	};
}