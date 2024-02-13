#pragma once
#include "Module.h"
#include "LogicalDevice.h"
namespace rttvk {
	class CommandPool : public Module {
	public:
		CommandPool(LogicalDevice* device);
		virtual void Create() override;
		virtual void Destroy() override;
		VkCommandPool GetPool();
	private:
		VkCommandPool pool;
		LogicalDevice* device;
	};
}