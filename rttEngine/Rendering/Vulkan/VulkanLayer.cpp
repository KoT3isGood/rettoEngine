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

	atrousShader = rttvk::Shader("Content/EngineLoad/Shaders/atrous.comp", &logicalDevice, VK_SHADER_STAGE_COMPUTE_BIT);
	atrousShader.Create();
	atrousPipeline.Create();

	pipeline.Create();
	computeShader.Destroy();

	commandPool.Create();
	commandBuffer.Create();


	renderFinished.Create();
	imageAvailable.Create();
	inFlightFence.Create();

	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	poolSize.descriptorCount = 9;

	VkDescriptorPoolSize poolSize2 = {};
	poolSize2.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize2.descriptorCount = 1;

	VkDescriptorPoolSize poolSize3 = {};
	poolSize3.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	poolSize3.descriptorCount = 1;

	VkDescriptorPoolSize poolSize4 = {};
	poolSize4.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSize4.descriptorCount = 1;

	VkDescriptorPoolSize poolSize5 = {};
	poolSize5.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSize5.descriptorCount = 1;

	VkDescriptorPoolSize poolSizes[] = { poolSize,poolSize2,poolSize3,poolSize4,poolSize5 };

	VkDescriptorPoolCreateInfo poolCreateInfo = {};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.poolSizeCount = 5;
	poolCreateInfo.pPoolSizes = poolSizes;
	poolCreateInfo.maxSets = 1;

	VK_CREATE_VALIDATION(vkCreateDescriptorPool(logicalDevice.GetDevice(), &poolCreateInfo, nullptr, &descPool));

	VkDescriptorSetAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.descriptorPool = descPool;
	allocateInfo.descriptorSetCount = 1;
	auto layout = pipeline.GetDescriptorLayout();
	allocateInfo.pSetLayouts = &layout;

	VK_CREATE_VALIDATION(vkAllocateDescriptorSets(logicalDevice.GetDevice(), &allocateInfo, &descSet));
	//buffer.Create();

	VkDescriptorPoolSize poolSizeAtrous = {};
	poolSizeAtrous.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	poolSizeAtrous.descriptorCount = 6;

	VkDescriptorPoolSize poolSizesAtrous[] = { poolSizeAtrous };

	poolCreateInfo.poolSizeCount = 1;
	poolCreateInfo.pPoolSizes = poolSizesAtrous;
	poolCreateInfo.maxSets = 1;

	VK_CREATE_VALIDATION(vkCreateDescriptorPool(logicalDevice.GetDevice(), &poolCreateInfo, nullptr, &descPoolAtrous));

	allocateInfo.descriptorPool = descPoolAtrous;
	allocateInfo.descriptorSetCount = 1;
	layout = atrousPipeline.GetDescriptorLayout();
	allocateInfo.pSetLayouts = &layout;

	VK_CREATE_VALIDATION(vkAllocateDescriptorSets(logicalDevice.GetDevice(), &allocateInfo, &descSetAtrous));



	resolutionBuffer = rttvk::Buffer(&logicalDevice, 12, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	resolutionBuffer.Create();
	






	
	rayGenShader = rttvk::Shader("Content/EngineLoad/Shaders/raytracer.rgen",&logicalDevice,VK_SHADER_STAGE_RAYGEN_BIT_KHR);
	rayGenShader.Create();


	missShader = rttvk::Shader("Content/EngineLoad/Shaders/raytracer.rmiss", &logicalDevice, VK_SHADER_STAGE_MISS_BIT_KHR);
	missShader.Create();

	shadowMissShader = rttvk::Shader("Content/EngineLoad/Shaders/raytracershadow.rmiss", &logicalDevice, VK_SHADER_STAGE_MISS_BIT_KHR);
	shadowMissShader.Create();

	closestHitShader = rttvk::Shader("Content/EngineLoad/Shaders/raytracer.rchit", &logicalDevice, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
	closestHitShader.Create();


	rtPipeline.Create();




	vkCmdTraceRaysKHR = (PFN_vkCmdTraceRaysKHR)vkGetDeviceProcAddr(logicalDevice.GetDevice(), "vkCmdTraceRaysKHR");


	VkDescriptorPoolSize poolSizeRT = {};
	poolSizeRT.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	poolSizeRT.descriptorCount = 11;

	VkDescriptorPoolSize poolSizeRT2 = {};
	poolSizeRT2.type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	poolSizeRT2.descriptorCount = 1;

	VkDescriptorPoolSize poolSizeRTUniform = {};
	poolSizeRTUniform.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizeRTUniform.descriptorCount = 3;

	VkDescriptorPoolSize poolSizeRTNoise = {};
	poolSizeRTNoise.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizeRTNoise.descriptorCount = 65;

	VkDescriptorPoolSize poolSizeRTStorage = {};
	poolSizeRTStorage.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSizeRTStorage.descriptorCount = 2;

	VkDescriptorPoolSize poolSizeRTs[] = { poolSizeRT,poolSizeRT2,poolSizeRTUniform,poolSizeRTNoise,poolSizeRTStorage };

	VkDescriptorPoolCreateInfo poolCreateInfoRT = {};
	poolCreateInfoRT.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfoRT.poolSizeCount = 5;
	poolCreateInfoRT.pPoolSizes = poolSizeRTs;
	poolCreateInfoRT.maxSets = 1;

	VK_CREATE_VALIDATION(vkCreateDescriptorPool(logicalDevice.GetDevice(), &poolCreateInfoRT, nullptr, &descPoolRT));

	VkDescriptorSetAllocateInfo allocateInfoRT = {};
	allocateInfoRT.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfoRT.descriptorPool = descPoolRT;
	allocateInfoRT.descriptorSetCount = 1;
	VkDescriptorSetLayout layoutRT = rtPipeline.GetDescriptorLayout();
	allocateInfoRT.pSetLayouts = &layoutRT;

	VK_CREATE_VALIDATION(vkAllocateDescriptorSets(logicalDevice.GetDevice(), &allocateInfoRT, &descSetRT));












	// Shader binding table
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(chosenPhysicalDevice,
		&physicalDeviceProperties);

	rtProperties = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR};

	VkPhysicalDeviceProperties2 physicalDeviceProperties2 {
		VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
		&rtProperties,
		physicalDeviceProperties
		};

	vkGetPhysicalDeviceProperties2(chosenPhysicalDevice,
		&physicalDeviceProperties2);

	// FIXME: Use better way to find sizes
	const uint32_t handleSize = rtProperties.shaderGroupHandleSize;
	const uint32_t handleSizeAligned = align(rtProperties.shaderGroupHandleSize, rtProperties.shaderGroupHandleAlignment);
	rgenRegion.size = handleSizeAligned;
	rgenRegion.stride = handleSizeAligned;
	rmissRegion.size = handleSizeAligned;
	rmissRegion.stride = handleSizeAligned;
	rchitRegion.size = handleSizeAligned;
	rchitRegion.stride = handleSizeAligned;
	
	shaderBindingTable = rttvk::Buffer(&logicalDevice, rgenRegion.size+ rmissRegion.size+ rchitRegion.size, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR);
	shaderBindingTable.Create();
	
	

	uint32_t             dataSize = 4 * handleSizeAligned;
	PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR = (PFN_vkGetRayTracingShaderGroupHandlesKHR)vkGetDeviceProcAddr(logicalDevice.GetDevice(), "vkGetRayTracingShaderGroupHandlesKHR");
	vkGetRayTracingShaderGroupHandlesKHR(logicalDevice.GetDevice(), rtPipeline.GetPipeline(), 0, 4, dataSize, shaderBindingTable.GetMapped());


	rgenRegion.deviceAddress = shaderBindingTable.GetBufferAddress();
	rmissRegion.deviceAddress = shaderBindingTable.GetBufferAddress()+32;
	rchitRegion.deviceAddress = shaderBindingTable.GetBufferAddress() + 64+32;


	// Some test meshes to test how tlas works

	Mesh testMesh = Mesh();
	testMesh.pos[0] = 0;
	testMesh.pos[1] = 0;
	testMesh.pos[2] = 0;

	
	
	meshes = { testMesh };


	// Creating and building acceleration structures
	blases.resize(1);
	MeshData meshDataNone = MeshData();
	OBJLoader objLoader = OBJLoader("Content/Meshes/none.obj", &meshDataNone);
	blases[0] = rttvk::BLAS(&logicalDevice, &meshDataNone);
	blases[0].Create();
	blases[0].Build();
	int i = 1;
	for (auto mesh: getProcessInfo()->assetRegistry.meshes) {
		if (mesh.first == "Content/Meshes/none.obj") {
			continue;
		}
		blases.resize(i+1);
		MeshData meshData = MeshData();
		OBJLoader objLoader = OBJLoader(mesh.first, &meshData);
		blases[i]=rttvk::BLAS(&logicalDevice,&meshData);
		blases[i].Create();
		blases[i].Build();
		i++;
	};


	tlas.Create();
	tlas.Build();

	blueNoise.Create();

	// TODO: MAKE SIZE DYNAMIC
	rttGUIDrawHierarchy = rttvk::Buffer(&logicalDevice, 60*1024, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	rttGUIDrawHierarchy.Create();

	bufferHierarchyAmount = rttvk::Buffer(&logicalDevice, 4, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	bufferHierarchyAmount.Create();
	memcpy(bufferHierarchyAmount.GetMapped(), &rttGUI()->id, 4);

	font.Create();
	cameraPositionBuffer.Create();
	lightsCountBuffer.Create();
	lightsBuffer = rttvk::Buffer(&logicalDevice, 32, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	lightsBuffer.Create();

	

	uint32_t texturesCounted = 0;
	imageInfos.resize(getProcessInfo()->assetRegistry.textures.size());
	for (auto assetTexture : getProcessInfo()->assetRegistry.textures) {
		rttvk::Texture text = rttvk::Texture(&logicalDevice, assetTexture.first);
		text.Create();
		textures.push_back(text);
		imageInfos[texturesCounted].imageView = text.imageView.GetImageView();
		imageInfos[texturesCounted].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageInfos[texturesCounted].sampler = text.textureSampler;
		texturesCounted++;
	}
	albedo.Create();
	direct.Create();
	indirect.Create();
	worldPos.Create();
	normal.Create();
	direct2.Create();
	indirect2.Create();
	directPrev.Create();
	indirectPrev.Create();
}

VulkanLayer::~VulkanLayer()
{


	vkDeviceWaitIdle(logicalDevice.GetDevice());

	for (auto blas : blases) {
		blas.Destroy();
	}

	for (auto texture : textures) {
		texture.Destroy();
	};

	rttGUIDrawHierarchy.Destroy();
	bufferHierarchyAmount.Destroy();
	cameraPositionBuffer.Destroy();
	font.Destroy();

	blueNoise.Destroy();

	tlas.Destroy();

	shadowMissShader.Destroy();
	rayGenShader.Destroy();
	missShader.Destroy();
	closestHitShader.Destroy();

	shaderBindingTable.Destroy();
	rtPipeline.Destroy();



	vkDestroyDescriptorPool(logicalDevice.GetDevice(), descPoolRT, nullptr);



	resolutionBuffer.Destroy();

	//buffer.Destroy();

	//vkUnmapMemory(logicalDevice.GetDevice(), memory);
	//vkDestroyBuffer(logicalDevice.GetDevice(), resbuffer, nullptr);

	//vkDestroyDescriptorSet(logicalDevice.GetDevice(), descSet, nullptr);
	vkDestroyDescriptorPool(logicalDevice.GetDevice(), descPool, nullptr);
	inFlightFence.Destroy();
	imageAvailable.Destroy();
	renderFinished.Destroy();
	commandPool.Destroy();
	pipeline.Destroy();
	
	if (swapchain.isOk)
	{
		for (int i = 0; i < imageViews.size(); i++) {
			imageViews[i].Destroy();
		}
		swapchain.Destroy();
	}
	surface.Destroy();
	logicalDevice.Destroy();
	debugMessenger.Destroy();
	instance.Destroy();
}

void VulkanLayer::ChangeImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkImageSubresourceRange access;
	access.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	access.baseMipLevel = 0;
	access.levelCount = 1;
	access.baseArrayLayer = 0;
	access.layerCount = 1;

	VkImageMemoryBarrier barrier;
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.srcAccessMask = VK_ACCESS_NONE_KHR;
	barrier.dstAccessMask = VK_ACCESS_NONE_KHR;
	barrier.image = image;
	barrier.subresourceRange = access;
	barrier.pNext = nullptr;


	vkCmdPipelineBarrier(commandBuffer.GetBuffer(), VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, 0, 0, nullptr, 0, nullptr, 1, &barrier);

}

void VulkanLayer::RecordCommandBuffer(uint32_t imageIndex)
{
	
	memcpy((void*)((uint64_t)cameraPositionBuffer.GetMapped()+48), pos, 48);

	pos[0][0] = getProcessInfo()->camera[0][0];
	pos[0][1] = getProcessInfo()->camera[0][1];
	pos[0][2] = getProcessInfo()->camera[0][2];
	pos[0][3] = getProcessInfo()->camera[0][3];
	pos[1][0] = getProcessInfo()->camera[1][0];
	pos[1][1] = getProcessInfo()->camera[1][1];
	pos[1][2] = getProcessInfo()->camera[1][2];
	pos[1][3] = getProcessInfo()->camera[1][3];
	pos[2][0] = getProcessInfo()->camera[2][0];
	pos[2][1] = getProcessInfo()->camera[2][1];
	pos[2][2] = getProcessInfo()->camera[2][2];
	pos[2][3] = getProcessInfo()->camera[2][3];
	memcpy(rttGUIDrawHierarchy.GetMapped(), rttGUI()->drawHierarchy.data(), rttGUI()->drawHierarchy.size() * 4);
	memcpy(bufferHierarchyAmount.GetMapped(), &rttGUI()->id, 4);
	memcpy(cameraPositionBuffer.GetMapped(), pos, 48);



	// Ray tracing pipeline
	memcpy(resolutionBuffer.GetMapped(), resolution, sizeof(resolution));
	memcpy((void*)((uint64_t)resolutionBuffer.GetMapped()+8), &win64surface->time,4);


	// TODO: Move this into constructor
	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageView = imageViews[imageIndex].GetImageView();
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkWriteDescriptorSet wds = {};
	wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wds.dstSet = descSetRT;
	wds.dstBinding = 0;
	wds.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	wds.descriptorCount = 1;
	wds.pImageInfo = &imageInfo;

	// TODO: Move this into constructor
	VkDescriptorImageInfo imageInfo2 = {};
	imageInfo2.imageView = albedo.imageView.GetImageView();
	imageInfo2.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkWriteDescriptorSet wds2 = {};
	wds2.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wds2.dstSet = descSetRT;
	wds2.dstBinding = 9;
	wds2.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	wds2.descriptorCount = 1;
	wds2.pImageInfo = &imageInfo2;

	VkDescriptorImageInfo imageInfo3 = {};
	imageInfo3.imageView = direct.imageView.GetImageView();
	imageInfo3.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkWriteDescriptorSet wds3 = {};
	wds3.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wds3.dstSet = descSetRT;
	wds3.dstBinding = 10;
	wds3.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	wds3.descriptorCount = 1;
	wds3.pImageInfo = &imageInfo3;

	VkDescriptorImageInfo imageInfo4 = {};
	imageInfo4.imageView = indirect.imageView.GetImageView();
	imageInfo4.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkWriteDescriptorSet wds4 = {};
	wds4.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wds4.dstSet = descSetRT;
	wds4.dstBinding = 11;
	wds4.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	wds4.descriptorCount = 1;
	wds4.pImageInfo = &imageInfo4;

	VkDescriptorImageInfo imageInfo5 = {};
	imageInfo5.imageView = normal.imageView.GetImageView();
	imageInfo5.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkWriteDescriptorSet wds5 = {};
	wds5.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wds5.dstSet = descSetRT;
	wds5.dstBinding = 12;
	wds5.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	wds5.descriptorCount = 1;
	wds5.pImageInfo = &imageInfo5;


	VkDescriptorImageInfo imageInfo6 = {};
	imageInfo6.imageView = worldPos.imageView.GetImageView();
	imageInfo6.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkWriteDescriptorSet wds6 = {};
	wds6.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wds6.dstSet = descSetRT;
	wds6.dstBinding = 13;
	wds6.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	wds6.descriptorCount = 1;
	wds6.pImageInfo = &imageInfo6;

	VkWriteDescriptorSetAccelerationStructureKHR wdsAc{};
	wdsAc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
	wdsAc.accelerationStructureCount = 1;
	wdsAc.pAccelerationStructures = tlas.GetAccelerationStructure();

	VkDescriptorImageInfo imageInfo7 = {};
	imageInfo7.imageView = direct2.imageView.GetImageView();
	imageInfo7.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkWriteDescriptorSet wds7 = {};
	wds7.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wds7.dstSet = descSetRT;
	wds7.dstBinding = 14;
	wds7.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	wds7.descriptorCount = 1;
	wds7.pImageInfo = &imageInfo7;


	VkDescriptorImageInfo imageInfo8 = {};
	imageInfo8.imageView = indirect2.imageView.GetImageView();
	imageInfo8.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkWriteDescriptorSet wds8 = {};
	wds8.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wds8.dstSet = descSetRT;
	wds8.dstBinding = 15;
	wds8.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	wds8.descriptorCount = 1;
	wds8.pImageInfo = &imageInfo8;

	VkDescriptorImageInfo imageInfo9 = {};
	imageInfo9.imageView = directPrev.imageView.GetImageView();
	imageInfo9.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkWriteDescriptorSet wds9 = {};
	wds9.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wds9.dstSet = descSetRT;
	wds9.dstBinding = 16;
	wds9.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	wds9.descriptorCount = 1;
	wds9.pImageInfo = &imageInfo9;

	VkDescriptorImageInfo imageInfo10 = {};
	imageInfo10.imageView = indirectPrev.imageView.GetImageView();
	imageInfo10.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkWriteDescriptorSet wds10 = {};
	wds10.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wds10.dstSet = descSetRT;
	wds10.dstBinding = 17;
	wds10.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	wds10.descriptorCount = 1;
	wds10.pImageInfo = &imageInfo10;

	VkWriteDescriptorSet wdsA = {};
	wdsA.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wdsA.dstSet = descSetRT;
	wdsA.dstBinding = 1;
	wdsA.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	wdsA.descriptorCount = 1;
	wdsA.pNext = &wdsAc;

	VkDescriptorBufferInfo resolutionInfo{};
	resolutionInfo.buffer = resolutionBuffer.GetBuffer();
	resolutionInfo.range = 12;


	VkWriteDescriptorSet wdsRes = {};
	wdsRes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wdsRes.dstSet = descSetRT;
	wdsRes.dstBinding = 2;
	wdsRes.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	wdsRes.descriptorCount = 1;
	wdsRes.pBufferInfo = &resolutionInfo;

	VkDescriptorImageInfo noiseImageInfo;
	noiseImageInfo.imageView = blueNoise.imageView.GetImageView();
	noiseImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	noiseImageInfo.sampler = blueNoise.textureSampler;

	VkWriteDescriptorSet wdsNoise = {};
	wdsNoise.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wdsNoise.dstSet = descSetRT;
	wdsNoise.dstBinding = 3;
	wdsNoise.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	wdsNoise.descriptorCount = 1;
	wdsNoise.pImageInfo = &noiseImageInfo;

	VkDescriptorBufferInfo camPosInfo{};
	camPosInfo.buffer = cameraPositionBuffer.GetBuffer();
	camPosInfo.range = 48*2;


	VkWriteDescriptorSet wdsCamPos = {};
	wdsCamPos.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wdsCamPos.dstSet = descSetRT;
	wdsCamPos.dstBinding = 4;
	wdsCamPos.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	wdsCamPos.descriptorCount = 1;
	wdsCamPos.pBufferInfo = &camPosInfo;

	VkDescriptorBufferInfo lightsCountInfo{};
	lightsCountInfo.buffer = lightsCountBuffer.GetBuffer();
	lightsCountInfo.range = 4;

	VkWriteDescriptorSet wdsLightsCount = {};
	wdsLightsCount.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wdsLightsCount.dstSet = descSetRT;
	wdsLightsCount.dstBinding = 5;
	wdsLightsCount.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	wdsLightsCount.descriptorCount = 1;
	wdsLightsCount.pBufferInfo = &lightsCountInfo;

	VkDescriptorBufferInfo lightsInfo{};
	lightsInfo.buffer = lightsBuffer.GetBuffer();
	lightsInfo.range = lights.size()*32;

	VkWriteDescriptorSet wdsLights = {};
	wdsLights.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wdsLights.dstSet = descSetRT;
	wdsLights.dstBinding = 6;
	wdsLights.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	wdsLights.descriptorCount = 1;
	wdsLights.pBufferInfo = &lightsInfo;

	VkDescriptorBufferInfo meshesInfo{};
	meshesInfo.buffer = tlas.blasesDataBuffer.GetBuffer();
	meshesInfo.range = blases.size()*48;

	VkWriteDescriptorSet wdsMeshes = {};
	wdsMeshes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wdsMeshes.dstSet = descSetRT;
	wdsMeshes.dstBinding = 7;
	wdsMeshes.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	wdsMeshes.descriptorCount = 1;
	wdsMeshes.pBufferInfo = &meshesInfo;

	VkWriteDescriptorSet wdsT = {};
	wdsT.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wdsT.dstSet = descSetRT;
	wdsT.dstBinding = 8;
	wdsT.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	wdsT.descriptorCount = getProcessInfo()->assetRegistry.textures.size();
	wdsT.pImageInfo = imageInfos.data();

	VkWriteDescriptorSet wdss[] = { wds ,wdsA, wdsRes, wdsNoise,wdsCamPos, wdsLightsCount, wdsLights, wdsMeshes, wdsT, wds2,wds3,wds4,wds5,wds6,wds9,wds10,wds7,wds8 };

	vkUpdateDescriptorSets(logicalDevice.GetDevice(), 18, wdss, 0, nullptr);

	VkDescriptorBufferInfo amountInfo{};
	amountInfo.buffer = bufferHierarchyAmount.GetBuffer();
	amountInfo.range = 4;



	VkWriteDescriptorSet wdsAmount = {};
	wdsAmount.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wdsAmount.dstSet = descSet;
	wdsAmount.dstBinding = 2;
	wdsAmount.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	wdsAmount.descriptorCount = 1;
	wdsAmount.pBufferInfo = &amountInfo;

	VkDescriptorBufferInfo hierarchyInfo{};
	hierarchyInfo.buffer = rttGUIDrawHierarchy.GetBuffer(); 
	hierarchyInfo.range = rttGUI()->drawHierarchy.size()*4;


	VkWriteDescriptorSet wdsHierarchy = {};
	wdsHierarchy.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wdsHierarchy.dstSet = descSet;
	wdsHierarchy.dstBinding = 3;
	wdsHierarchy.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	wdsHierarchy.descriptorCount = 1;
	wdsHierarchy.pBufferInfo = &hierarchyInfo;

	wds.dstSet = descSet;
	wds2.dstSet = descSet;
	wds3.dstSet = descSet;
	wds4.dstSet = descSet;
	wds7.dstSet = descSet;
	wds8.dstSet = descSet;
	wds9.dstSet = descSet;
	wds10.dstSet = descSet;

	wdsRes.dstSet = descSet;
	wdsRes.dstBinding = 1;

	VkDescriptorImageInfo fontImageInfo;
	fontImageInfo.imageView = font.imageView.GetImageView();
	fontImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	fontImageInfo.sampler = font.textureSampler;

	VkWriteDescriptorSet wdsFont = {};
	wdsFont.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wdsFont.dstSet = descSet;
	wdsFont.dstBinding = 4;
	wdsFont.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	wdsFont.descriptorCount = 1;
	wdsFont.pImageInfo = &fontImageInfo;

	VkWriteDescriptorSet wdss2[] = { wds, wdsRes, wdsAmount, wdsHierarchy,wdsFont, wds2,wds3,wds3,wds4,wds9,wds10,wds7,wds8 };
	vkUpdateDescriptorSets(logicalDevice.GetDevice(), 13, wdss2, 0, nullptr);

	wds3.dstSet = descSetAtrous;
	wds4.dstSet = descSetAtrous;
	wds7.dstSet = descSetAtrous;
	wds8.dstSet = descSetAtrous;
	wds5.dstSet = descSetAtrous;
	wds6.dstSet = descSetAtrous;


	

	VkWriteDescriptorSet wdssAtrous[] = { wds3,wds4, wds5,wds6, wds7,wds8 };
	vkUpdateDescriptorSets(logicalDevice.GetDevice(), 6, wdssAtrous, 0, nullptr);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkBeginCommandBuffer(commandBuffer.GetBuffer(), &beginInfo);
	
	tlas.Update(&commandBuffer);

	ChangeImageLayout(images[imageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
	vkCmdBindPipeline(commandBuffer.GetBuffer(), VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtPipeline.GetPipeline());
	vkCmdBindDescriptorSets(commandBuffer.GetBuffer(), VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtPipeline.GetPipelineLayout(), 0, 1, &descSetRT, 0, nullptr);

	VkStridedDeviceAddressRegionKHR sbt_null{};
	
	vkCmdTraceRaysKHR(commandBuffer.GetBuffer(), &rgenRegion, &rmissRegion, &rchitRegion, &sbt_null, resolution[0], resolution[1], 1);

	vkCmdBindPipeline(commandBuffer.GetBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, atrousPipeline.GetPipeline());
	vkCmdBindDescriptorSets(commandBuffer.GetBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, atrousPipeline.GetPipelineLayout(), 0, 1, &descSetAtrous, 0, nullptr);
	/*int atrousSize = 1;
	vkCmdPushConstants(commandBuffer.GetBuffer(), atrousPipeline.GetPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, 4, &atrousSize);
	vkCmdDispatch(commandBuffer.GetBuffer(), resolution[0]/32.0+1, resolution[1] / 32.0 + 1, 1);
	atrousSize = 2;
	vkCmdPushConstants(commandBuffer.GetBuffer(), atrousPipeline.GetPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, 4, &atrousSize);
	vkCmdDispatch(commandBuffer.GetBuffer(), resolution[0] / 32.0 + 1, resolution[1] / 32.0 + 1, 1);
	atrousSize = 3;
	vkCmdPushConstants(commandBuffer.GetBuffer(), atrousPipeline.GetPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, 4, &atrousSize);
	vkCmdDispatch(commandBuffer.GetBuffer(), resolution[0] / 32.0 + 1, resolution[1] / 32.0 + 1, 1);
	atrousSize = 4;
	vkCmdPushConstants(commandBuffer.GetBuffer(), atrousPipeline.GetPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, 4, &atrousSize);
	vkCmdDispatch(commandBuffer.GetBuffer(), resolution[0] / 32.0 + 1, resolution[1] / 32.0 + 1, 1);
	atrousSize = 5;
	vkCmdPushConstants(commandBuffer.GetBuffer(), atrousPipeline.GetPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, 4, &atrousSize);
	vkCmdDispatch(commandBuffer.GetBuffer(), resolution[0] / 32.0 + 1, resolution[1] / 32.0 + 1, 1);
	atrousSize = 6;
	vkCmdPushConstants(commandBuffer.GetBuffer(), atrousPipeline.GetPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, 4, &atrousSize);
	vkCmdDispatch(commandBuffer.GetBuffer(), resolution[0] / 32.0 + 1, resolution[1] / 32.0 + 1, 1);*/



	vkCmdBindPipeline(commandBuffer.GetBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.GetPipeline());
	vkCmdBindDescriptorSets(commandBuffer.GetBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.GetPipelineLayout(), 0, 1, &descSet, 0, nullptr);
	vkCmdDispatch(commandBuffer.GetBuffer(), resolution[0] / 32.0 + 1, resolution[1] / 32.0 + 1, 1);

	//ChangeImageLayout(images[imageIndex], VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	vkEndCommandBuffer(commandBuffer.GetBuffer());
}
void VulkanLayer::Draw()
{

	// FIXME
	//RTT_LOG("NewFrame");
	//rttGUIDrawHierarchy.Destroy();
	//rttGUIDrawHierarchy = rttvk::Buffer(&logicalDevice,rttGUI()->drawHierarchy.size()*4+8,VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	//rttGUIDrawHierarchy.Create();
	vkDeviceWaitIdle(logicalDevice.GetDevice());
	if (shouldResize) {
		albedo.Destroy();
		albedo = rttvk::Image(&logicalDevice, resolution[0], resolution[1], VK_FORMAT_R32G32B32_SFLOAT);
		albedo.Create();

		indirect.Destroy();
		indirect = rttvk::Image(&logicalDevice, resolution[0], resolution[1], VK_FORMAT_R32G32B32_SFLOAT);
		indirect.Create();

		direct.Destroy();
		direct = rttvk::Image(&logicalDevice, resolution[0], resolution[1], VK_FORMAT_R32G32B32_SFLOAT);
		direct.Create();

		direct2.Destroy();
		direct2 = rttvk::Image(&logicalDevice, resolution[0], resolution[1], VK_FORMAT_R32G32B32_SFLOAT);
		direct2.Create();

		indirect2.Destroy();
		indirect2 = rttvk::Image(&logicalDevice, resolution[0], resolution[1], VK_FORMAT_R32G32B32_SFLOAT);
		indirect2.Create();

		worldPos.Destroy();
		worldPos = rttvk::Image(&logicalDevice, resolution[0], resolution[1], VK_FORMAT_R32G32B32_SFLOAT);
		worldPos.Create();

		normal.Destroy();
		normal = rttvk::Image(&logicalDevice, resolution[0], resolution[1], VK_FORMAT_R32G32B32_SFLOAT);
		normal.Create();

		directPrev.Destroy();
		directPrev = rttvk::Image(&logicalDevice, resolution[0], resolution[1], VK_FORMAT_R32G32B32_SFLOAT);
		directPrev.Create();

		indirectPrev.Destroy();
		indirectPrev = rttvk::Image(&logicalDevice, resolution[0], resolution[1], VK_FORMAT_R32G32B32_SFLOAT);
		indirectPrev.Create();
		shouldResize = false;
	}
	tlas.Recreate();

	lightsBuffer.Destroy();
	lightsBuffer = rttvk::Buffer(&logicalDevice, 32 + 32 * lights.size(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	lightsBuffer.Create();
	for (auto light : lights) {
		lightsShader.push_back(LightSourceShader{ light.pos[0],light.pos[1] ,light.pos[2],light.size,light.color[0],light.color[1] ,light.color[2],0.0 });
	}
	uint32_t lightsCount = lights.size();
	memcpy(lightsCountBuffer.GetMapped(), &lightsCount, 4);
	memcpy(lightsBuffer.GetMapped(), lightsShader.data(), 32 * lights.size());
	lightsShader = {};

	vkWaitForFences(logicalDevice.GetDevice(), 1, inFlightFence.GetFenceP(), VK_TRUE, UINT64_MAX);
	vkResetFences(logicalDevice.GetDevice(), 1, inFlightFence.GetFenceP());

	uint32_t imageIndex = 0;
	VkResult result = vkAcquireNextImageKHR(logicalDevice.GetDevice(), swapchain.GetSwapchain(), UINT64_MAX, imageAvailable.GetSemaphore(), inFlightFence.GetFence(), &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		Resize();
		return;
	}

	vkResetCommandBuffer(commandBuffer.GetBuffer(), 0);
	RecordCommandBuffer(imageIndex);
	VkCommandBuffer cb = commandBuffer.GetBuffer();

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = { imageAvailable.GetSemaphore() };
	VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = &waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cb;

	VkSemaphore signalSemaphores[] = { renderFinished.GetSemaphore() };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	VK_VALIDATE(vkQueueSubmit(logicalDevice.GetGraphicsQueue(), 1, &submitInfo, *inFlightFence.GetFenceP()), vkQueueSubmit);

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapchains[] = { swapchain.GetSwapchain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;
	result = vkQueuePresentKHR(logicalDevice.GetPresentQueue(), &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		Resize();
	}
	ResetMeshes();
}

void VulkanLayer::Resize()
{
	swapchain.Destroy();
	for (int i = 0; i < imageViews.size(); i++) {
		imageViews[i].Destroy();
	}
	swapchain.Create();
	if (!swapchain.isOk)
	{
		return;
	}
	uint32_t imageCount = 0;
	vkGetSwapchainImagesKHR(logicalDevice.GetDevice(), swapchain.GetSwapchain(), &imageCount, nullptr);
	images.resize(imageCount);
	imageViews.resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice.GetDevice(), swapchain.GetSwapchain(), &imageCount, images.data());

	for (int i = 0; i < imageViews.size(); i++) {
		imageViews[i] = rttvk::ImageView(&logicalDevice, images[0]);
		imageViews[i].Create();
	}
	shouldResize = true;
}

