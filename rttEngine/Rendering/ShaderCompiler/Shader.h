#pragma once
#include <filesystem>
#include <string>
#include <fstream>

#include "Utils\Application\ProcessInfo.h"

#include "Rendering\Vulkan\Modules\LogicalDevice.h"
#include "Rendering\Vulkan\Modules\Module.h"

namespace rttvk {
	class Shader: public Module {
	public:
		Shader() = default;
		Shader(std::string filepath, LogicalDevice* device, VkShaderStageFlagBits stage);
		// TODO: Add loading from registry


		std::string assemblyPath;
		virtual void Create() override;
		virtual void Destroy() override;

		LogicalDevice* device;
		VkShaderStageFlagBits stage;
		VkShaderModule shaderModule;
		VkPipelineShaderStageCreateInfo shaderStageInfo{};
	};
}
