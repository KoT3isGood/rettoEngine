#include "Shader.h"

Shader::Shader(std::string filepath)
{
	std::filesystem::path extension = std::filesystem::path(filepath).extension();
	std::filesystem::path fileOutput = std::filesystem::path(filepath).filename();

	assemblyPath = filepath;

	if (!std::filesystem::exists("Build\\Unused\\Shaders\\")) {
		std::filesystem::create_directories("Build\\Unused\\Shaders\\");
	}
	if (extension != ".spv") {
		std::filesystem::path currentPath = std::filesystem::current_path();

		std::string outputPath = currentPath.string() + "/Content/EngineLoad/ShadersCompiled/" + fileOutput.string() + ".spv";
		std::string command = "%VULKAN_SDK%/Bin/glslc.exe " + filepath + " -o " + outputPath;
		RTT_LOG("Compiling shader: "+ outputPath);
		system(command.c_str());
		assemblyPath = outputPath;
	}
}





void Shader::loadAssemblies(VkDevice device, VkShaderStageFlagBits stage)
{
	this->device = device;
	if (!std::filesystem::exists(assemblyPath)) {
		RTT_LOG("Failed to find shader: " + assemblyPath);
		return;
	}




	std::ifstream file(assemblyPath, std::ios::ate | std::ios::binary);

	RTT_ASSERT(file.is_open());

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> assembly(fileSize);

	file.seekg(0);
	file.read(assembly.data(), fileSize);

	file.close();

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = assembly.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(assembly.data());


	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		RTT_LOG("Failed to create shader module!");
		RTT_ASSERT(false);
	}


	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.stage = stage;
	shaderStageInfo.module = shaderModule;
	shaderStageInfo.pName = "main";
}
