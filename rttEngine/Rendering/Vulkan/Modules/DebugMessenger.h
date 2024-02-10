#pragma once

#include "Module.h"
#include "Instance.h"

namespace rttvk {
	class DebugMessenger :public Module {
	public:
		DebugMessenger(Instance* instance);
		virtual void Create() override;
		virtual void Destroy() override;
	private:
		static VkBool32 VulkanCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData) {
			RTT_LOG(pCallbackData->pMessage);
			return VK_FALSE;
		}
		VkDebugUtilsMessengerEXT debugMessenger;
		Instance* instance;
	};
}