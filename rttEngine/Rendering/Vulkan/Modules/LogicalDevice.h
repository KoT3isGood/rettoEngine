#pragma once
#include "Module.h"
#include <vector>

namespace rttvk {
	class LogicalDevice : public Module {
	public:
		LogicalDevice(VkPhysicalDevice* physicalDevice, std::vector<const char*> extensions, std::vector<const char*> layers);
		virtual void Create() override;
		virtual void Destroy() override;
		VkDevice GetDevice();
	private:
		VkDevice device;
		std::vector<const char*> extensions;
		std::vector<const char*> layers;
		VkPhysicalDevice* physicalDevice;
	};

}