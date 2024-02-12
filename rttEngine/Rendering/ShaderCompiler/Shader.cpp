#include "Shader.h"

namespace rttvk {
	Shader::Shader(std::string filepath, LogicalDevice* device, VkShaderStageFlagBits stage)
	{
		this->stage = stage;
		this->device = device;

		std::filesystem::path extension = std::filesystem::path(filepath).extension();
		std::filesystem::path fileOutput = std::filesystem::path(filepath).filename();

		assemblyPath = filepath;

		if (!std::filesystem::exists("Build\\Unused\\Shaders\\")) {
			std::filesystem::create_directories("Build\\Unused\\Shaders\\");
		}
		if (extension != ".spv") {
			std::filesystem::path currentPath = std::filesystem::current_path();

			std::string outputPath = currentPath.string() + "/Content/EngineLoad/ShadersCompiled/" + fileOutput.string() + ".spv";
			std::string command = "%VULKAN_SDK%/Bin/glslc.exe " + filepath + " --target-spv=spv1.5 -o " + outputPath;
			RTT_LOG("[ SPIRV ] Compiling shader: " + outputPath);
			system(command.c_str());
			assemblyPath = outputPath;
		}
	}





	void Shader::Create()
	{
		this->device = device;
		if (!std::filesystem::exists(assemblyPath)) {
			RTT_LOG("[ SPIRV ] Failed to find shader: " + assemblyPath);
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


		VK_CREATE_VALIDATION(vkCreateShaderModule(device->GetDevice(), &createInfo, nullptr, &shaderModule), VkShaderModule);


		shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageInfo.stage = stage;
		shaderStageInfo.module = shaderModule;
		shaderStageInfo.pName = "main";
	}
	void Shader::Destroy() {
		vkDestroyShaderModule(device->GetDevice(), shaderModule, nullptr);
	}
}