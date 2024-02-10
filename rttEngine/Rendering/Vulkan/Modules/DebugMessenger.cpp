#include "DebugMessenger.h"

namespace rttvk {
	DebugMessenger::DebugMessenger(Instance* instance)
	{
		this->instance = instance;
	}
	void DebugMessenger::Create()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
		createInfo.pfnUserCallback = VulkanCallback;

		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance->GetInstance(), "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			VK_CREATE_VALIDATION(func(instance->GetInstance(), &createInfo, nullptr, &debugMessenger), VkDebugUtilsMessengerEXT);
			return;
		}
		RTT_ASSERT(0);
	}

	void DebugMessenger::Destroy()
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance->GetInstance(), "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance->GetInstance(), debugMessenger, nullptr);
			return;
		}
	}
}
