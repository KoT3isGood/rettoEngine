#pragma once
#include "Vulkan\vulkan.h"
#include "Runtime\Logging\Logger.h"
#define VK_CREATE_VALIDATION(result, vkStruct) RTT_LOG(std::string("CREATING ")+#vkStruct);if(result!=VK_SUCCESS) {RTT_LOG(std::string("FAILED TO CREATE ")+#vkStruct);RTT_ASSERT(0);}

namespace rttvk {
	class Module {
	public:
		virtual void Create();
		virtual void Destroy();
	};
}