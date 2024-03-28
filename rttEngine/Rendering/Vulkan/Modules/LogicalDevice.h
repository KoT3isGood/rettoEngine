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
		VkPhysicalDevice* GetPhysicalDevice();
		VkQueue GetGraphicsQueue();
		VkQueue GetPresentQueue();

		uint32_t GetGraphicsQueueIndex();
		uint32_t GetPresentQueueIndex();
	private:
		VkDevice device;
		std::vector<const char*> extensions;
		std::vector<const char*> layers;
		VkPhysicalDevice* physicalDevice;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		uint32_t graphicsFamily = 0;
		bool isGraphicsFamilyDone = false;
		uint32_t presentFamily = 0;
		bool isPresentFamilyDone = false;
	};

}