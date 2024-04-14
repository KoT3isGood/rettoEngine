#pragma once
#include "Vulkan\vulkan.h"
#include "Runtime\Logging\Logger.h"
#define VK_CREATE_VALIDATION(result) if(result!=VK_SUCCESS) {RTT_ASSERT(0);}
#define VK_FUNCTION(func,device) PFN_##func func = (PFN_##func)vkGetDeviceProcAddr(device,#func)
namespace rttvk {
	class Module {
	public:
		virtual void Create();
		virtual void Destroy();
	};
}