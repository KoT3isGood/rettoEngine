#include "Pipeline.h"

namespace rttvk{
	Pipeline::Pipeline(Shader* shader, LogicalDevice* device)
	{
		
		this->shader = shader;
		this->device = device;
	}
	void Pipeline::Create()
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		VkDescriptorSetLayoutBinding binding{};
		binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutBinding binding2{};
		binding2.binding = 1;
		binding2.descriptorCount = 1;
		binding2.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		binding2.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;


		bindings.push_back(binding);
		bindings.push_back(binding2);

		VkDescriptorSetLayoutCreateInfo descriptorInfo{};
		descriptorInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorInfo.bindingCount = bindings.size();
		descriptorInfo.pBindings = bindings.data();

		VK_CREATE_VALIDATION(vkCreateDescriptorSetLayout(device->GetDevice(), &descriptorInfo, nullptr, &descriptor), VkDescriptorSetLayout);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pSetLayouts = &descriptor;
		pipelineLayoutInfo.setLayoutCount = 1;
		VK_CREATE_VALIDATION(vkCreatePipelineLayout(device->GetDevice(), &pipelineLayoutInfo, nullptr, &layout), VkPipelineLayout);



		VkPipelineCacheCreateInfo cacheCreateInfo{};
		cacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		VK_CREATE_VALIDATION(vkCreatePipelineCache(device->GetDevice(), &cacheCreateInfo, nullptr, &cache), VkPipelineCache);

		VkComputePipelineCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		createInfo.layout = layout;
		createInfo.stage = shader->shaderStageInfo;


		VK_CREATE_VALIDATION(vkCreateComputePipelines(device->GetDevice(), cache, 1, &createInfo, nullptr, &pipeline), VkPipeline);
	}
	void Pipeline::Destroy()
	{
		
		vkDestroyPipeline(device->GetDevice(), pipeline, nullptr);
		vkDestroyPipelineCache(device->GetDevice(), cache, nullptr);
		vkDestroyPipelineLayout(device->GetDevice(), layout, nullptr);
		vkDestroyDescriptorSetLayout(device->GetDevice(), descriptor, nullptr);
	}
	VkPipeline Pipeline::GetPipeline()
	{
		return pipeline;
	}
	VkDescriptorSetLayout Pipeline::GetDescriptorLayout()
	{
		return descriptor;
	}
	VkPipelineLayout Pipeline::GetPipelineLayout()
	{
		return layout;
	}
}

