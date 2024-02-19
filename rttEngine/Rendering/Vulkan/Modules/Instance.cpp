#include "Instance.h"
namespace rttvk {
	Instance::Instance(std::vector<const char*> extensions, std::vector<const char*> layers)
	{
		this->extensions = extensions;
		this->layers = layers;
	}
	void Instance::Create()
	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.apiVersion = VK_API_VERSION_1_2;

		VkInstanceCreateInfo createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = extensions.size();
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.enabledLayerCount = layers.size();
		createInfo.ppEnabledLayerNames = layers.data();


		VK_CREATE_VALIDATION(vkCreateInstance(&createInfo,nullptr,&instance), VkInstance);
	}
	void Instance::Destroy()
	{
		vkDestroyInstance(instance, nullptr);
	}
	VkInstance Instance::GetInstance()
	{
		return instance;
	}
}

