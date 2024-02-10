#pragma once
#include <filesystem>
#include <string>
#include <fstream>
#include "Runtime\Logging\Logger.h"
#include "vulkan\vulkan.h"

#include "Runtime\Application\ProcessInfo.h"
class Shader {
public:
	Shader(std::string filepath);
	// TODO: Add loading from registry


	std::string assemblyPath;
	void loadAssemblies(VkDevice device, VkShaderStageFlagBits stage);

	VkDevice device;

	VkShaderModule shaderModule;
	VkPipelineShaderStageCreateInfo shaderStageInfo{};
};