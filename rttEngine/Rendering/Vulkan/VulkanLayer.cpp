#include "VulkanLayer.h"

VulkanLayer::VulkanLayer()
{

	instance.Create();
	debugMessenger.Create();

	// Selecting Physical Device
	// TODO: IMPLEMENT BETTER SYSTEM
	uint32_t pdCount = 0;
	vkEnumeratePhysicalDevices(instance.GetInstance(), &pdCount, nullptr);
	RTT_ASSERT(pdCount);
	std::vector<VkPhysicalDevice> physicalDevices(pdCount);
	vkEnumeratePhysicalDevices(instance.GetInstance(), &pdCount, physicalDevices.data());
	chosenPhysicalDevice = physicalDevices[0];

	logicalDevice.Create();

	surface = rttvk::SurfaceKHR(&instance, win64surface->Handle, win64surface->instance);
	surface.Create();
	swapchain.Create();

	// Get Image Count
	uint32_t imageCount = 0;
	vkGetSwapchainImagesKHR(logicalDevice.GetDevice(), swapchain.GetSwapchain(), &imageCount, nullptr);
	images.resize(imageCount);
	imageViews.resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice.GetDevice(), swapchain.GetSwapchain(), &imageCount, images.data());

	for (int i = 0; i < imageViews.size(); i++) {
		imageViews[i] = rttvk::ImageView(&logicalDevice, images[0]);
		imageViews[i].Create();
	}

	computeShader = rttvk::Shader("Content/EngineLoad/Shaders/shader.comp", &logicalDevice, VK_SHADER_STAGE_COMPUTE_BIT);
	computeShader.Create();

	pipeline.Create();
	computeShader.Destroy();

	commandPool.Create();
	commandBuffer.Create();


	renderFinished.Create();
	imageAvailable.Create();
	inFlightFence.Create();

	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	poolSize.descriptorCount = 1;

	VkDescriptorPoolSize poolSize2 = {};
	poolSize2.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize2.descriptorCount = 1;

	VkDescriptorPoolSize poolSizes[] = { poolSize,poolSize2 };

	VkDescriptorPoolCreateInfo poolCreateInfo = {};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.poolSizeCount = 2;
	poolCreateInfo.pPoolSizes = poolSizes;
	poolCreateInfo.maxSets = 1;

	VK_CREATE_VALIDATION(vkCreateDescriptorPool(logicalDevice.GetDevice(), &poolCreateInfo, nullptr, &descPool),VkDescriptorPool);

	VkDescriptorSetAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.descriptorPool = descPool;
	allocateInfo.descriptorSetCount = 1;
	auto layout = pipeline.GetDescriptorLayout();
	allocateInfo.pSetLayouts = &layout;

	VK_CREATE_VALIDATION(vkAllocateDescriptorSets(logicalDevice.GetDevice(), &allocateInfo, &descSet), VkDescriptorSet);
	buffer.Create();

	
}

VulkanLayer::~VulkanLayer()
{
	vkDeviceWaitIdle(logicalDevice.GetDevice());

	buffer.Destroy();

	//vkUnmapMemory(logicalDevice.GetDevice(), memory);
	//vkDestroyBuffer(logicalDevice.GetDevice(), resbuffer, nullptr);

	//vkDestroyDescriptorSet(logicalDevice.GetDevice(), descSet, nullptr);
	vkDestroyDescriptorPool(logicalDevice.GetDevice(), descPool, nullptr);
	inFlightFence.Destroy();
	imageAvailable.Destroy();
	renderFinished.Destroy();
	commandPool.Destroy();
	pipeline.Destroy();
	for (int i = 0; i < imageViews.size(); i++) {
		imageViews[i].Destroy();
	}
	swapchain.Destroy();
	surface.Destroy();
	logicalDevice.Destroy();
	debugMessenger.Destroy();
	instance.Destroy();
}

void VulkanLayer::RecordCommandBuffer(uint32_t imageIndex)
{
	memcpy(buffer.GetMapped(), resolutionBuf, sizeof(resolutionBuf));

	VkPipelineStageFlags sourceStage = 0, destinationStage = 0;

	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageView = imageViews[imageIndex].GetImageView();
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkWriteDescriptorSet writeDescriptorSet = {};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstSet = descSet;
	writeDescriptorSet.dstBinding = 0;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	writeDescriptorSet.descriptorCount = 1;

	writeDescriptorSet.pImageInfo = &imageInfo;

	
	VkDescriptorBufferInfo dbi;
	dbi.buffer = buffer.GetBuffer();
	dbi.offset = 0;
	dbi.range = 8;

	VkWriteDescriptorSet writeDescriptorSet2 = {};
	writeDescriptorSet2.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet2.dstSet = descSet;
	writeDescriptorSet2.dstBinding = 1;
	writeDescriptorSet2.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeDescriptorSet2.descriptorCount = 1;
	writeDescriptorSet2.pBufferInfo = &dbi;
	std::vector<VkWriteDescriptorSet> writedescsets;
	writedescsets.push_back(writeDescriptorSet);
	writedescsets.push_back(writeDescriptorSet2);

	vkUpdateDescriptorSets(logicalDevice.GetDevice(), writedescsets.size(), writedescsets.data(), 0, nullptr);



	



	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkBeginCommandBuffer(commandBuffer.GetBuffer(), &beginInfo);
	
	//vkCmdUpdateBuffer(commandBuffer.GetBuffer(), resbuffer,0,8, resolutionBuf);
	//vkCmdPipelineBarrier(commandBuffer.GetBuffer(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier2);
	vkCmdBindPipeline(commandBuffer.GetBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.GetPipeline());
	vkCmdBindDescriptorSets(commandBuffer.GetBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.GetPipelineLayout(), 0, 1, &descSet, 0, nullptr);
	vkCmdDispatch(commandBuffer.GetBuffer(), 1280*0.125, 720 * 0.125, 1);
	//vkCmdPipelineBarrier(commandBuffer.GetBuffer(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	vkEndCommandBuffer(commandBuffer.GetBuffer());
}
void VulkanLayer::Draw()
{
	
	RTT_LOG("---------------------------------------------------\n new frame\n---------------------------------------------------");
	vkWaitForFences(logicalDevice.GetDevice(), 1, inFlightFence.GetFenceP(), VK_TRUE, UINT64_MAX);
	vkResetFences(logicalDevice.GetDevice(), 1, inFlightFence.GetFenceP());
	//RTT_LOG("1");
	uint32_t imageIndex = 0;
	vkAcquireNextImageKHR(logicalDevice.GetDevice(), swapchain.GetSwapchain(), UINT64_MAX, imageAvailable.GetSemaphore(), inFlightFence.GetFence(), &imageIndex);
	//RTT_LOG("CurrentImage: "+ std::to_string(imageIndex));

	vkResetCommandBuffer(commandBuffer.GetBuffer(), 0);

	//RTT_LOG("2");
	RecordCommandBuffer(imageIndex);
	//RTT_LOG("3");
	VkCommandBuffer cb = commandBuffer.GetBuffer();

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = { imageAvailable.GetSemaphore() };
	VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = &waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cb;
	VkSemaphore signalSemaphores[] = { renderFinished.GetSemaphore() };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	//vkWaitForFences(logicalDevice.GetDevice(), 1, &fence, VK_FALSE, UINT64_MAX);
	VK_VALIDATE(vkQueueSubmit(logicalDevice.GetGraphicsQueue(), 1, &submitInfo, inFlightFence.GetFence()), vkQueueSubmit);

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapchains[] = { swapchain.GetSwapchain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;
	vkQueuePresentKHR(logicalDevice.GetPresentQueue(), &presentInfo);
}

