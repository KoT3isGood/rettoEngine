#pragma once
#include "Module.h"
#include "LogicalDevice.h"
#include "CommandPool.h"
namespace rttvk {
	class CommandBuffer:public Module {
	public:
		CommandBuffer(LogicalDevice* device, CommandPool* pool);
		virtual void Create() override;
		VkCommandBuffer GetBuffer();
		VkCommandBuffer* GetBufferP();
	private:
		LogicalDevice* device;
		CommandPool* pool;
		VkCommandBuffer buffer;
	};
}