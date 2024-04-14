#include "RTPipeline.h"
namespace rttvk {
	RTPipeline::RTPipeline(LogicalDevice* device, std::vector<Shader*> shaders, std::vector<VkDescriptorSetLayoutBinding> descSetlayout)
	{
		this->device = device;
		this->shaders = shaders;
		this->descSetlayout = descSetlayout;
	}

	void RTPipeline::Create()
	{
		vkCreateRayTracingPipelinesKHR = (PFN_vkCreateRayTracingPipelinesKHR)vkGetDeviceProcAddr(device->GetDevice(), "vkCreateRayTracingPipelinesKHR");


		for (auto& binding : descSetlayout) {
			binding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		}

		VkDescriptorSetLayoutCreateInfo descriptorInfo{};
		descriptorInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorInfo.bindingCount = descSetlayout.size();
		descriptorInfo.pBindings = descSetlayout.data();

		VK_CREATE_VALIDATION(vkCreateDescriptorSetLayout(device->GetDevice(), &descriptorInfo, nullptr, &descriptor));

		VkPipelineLayoutCreateInfo layoutCreateInfo{};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutCreateInfo.setLayoutCount = 1;
		layoutCreateInfo.pSetLayouts = &descriptor;
		VK_CREATE_VALIDATION(vkCreatePipelineLayout(device->GetDevice(), &layoutCreateInfo, nullptr, &layout));

		VkRayTracingPipelineCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
		std::vector<VkPipelineShaderStageCreateInfo> stages;

		for (auto shader : shaders) {
			stages.push_back(shader->shaderStageInfo);
		};

		createInfo.stageCount = stages.size();
		createInfo.pStages = stages.data();


		std::vector<VkRayTracingShaderGroupCreateInfoKHR> groups{};
		VkRayTracingShaderGroupCreateInfoKHR group{};
		group.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;

		group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
		group.anyHitShader = VK_SHADER_UNUSED_KHR;
		group.closestHitShader = VK_SHADER_UNUSED_KHR;
		group.generalShader = 0;
		group.intersectionShader = VK_SHADER_UNUSED_KHR;
		groups.push_back(group);

		group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
		group.anyHitShader = VK_SHADER_UNUSED_KHR;
		group.closestHitShader = VK_SHADER_UNUSED_KHR;
		group.generalShader = 1;
		group.intersectionShader = VK_SHADER_UNUSED_KHR;
		groups.push_back(group);

		group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
		group.anyHitShader = VK_SHADER_UNUSED_KHR;
		group.closestHitShader = 2;
		group.generalShader = VK_SHADER_UNUSED_KHR;
		group.intersectionShader = VK_SHADER_UNUSED_KHR;
		groups.push_back(group);


		createInfo.groupCount = groups.size();
		createInfo.pGroups = groups.data();
		createInfo.maxPipelineRayRecursionDepth = 1;
		createInfo.layout = layout;

		VK_CREATE_VALIDATION(vkCreateRayTracingPipelinesKHR(device->GetDevice(),nullptr,nullptr,1,&createInfo,nullptr,&pipeline));
	}

	void RTPipeline::Destroy()
	{
		vkDestroyPipeline(device->GetDevice(), pipeline, nullptr);
		vkDestroyPipelineLayout(device->GetDevice(), layout, nullptr);
		vkDestroyDescriptorSetLayout(device->GetDevice(), descriptor, nullptr);

	}

	VkPipeline RTPipeline::GetPipeline()
	{
		return pipeline;
	}

	VkDescriptorSetLayout RTPipeline::GetDescriptorLayout()
	{
		return descriptor;
	}

	VkPipelineLayout RTPipeline::GetPipelineLayout()
	{
		return layout;
	}

}
