#include "TLAS.h"
namespace rttvk {
	TLAS::TLAS(LogicalDevice* device, BLAS* blas)
	{
		this->device = device;
		this->blas = blas;
		
	}
	void TLAS::Create()
	{
		
		instance.transform = VkTransformMatrixKHR{
			1,0,0,0,
			0,1,0,0,
			0,0,1,0
		};
		instance.instanceCustomIndex = 0;
		instance.accelerationStructureReference = blas->GetAccelerationStructureAddress();
		instance.flags = 0;
		instance.mask = 0xFF;
		instance.instanceShaderBindingTableRecordOffset = 0;

		instanceBuffer = Buffer(device, sizeof(VkAccelerationStructureInstanceKHR),
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR
		);
		instanceBuffer.Create();
		memcpy(instanceBuffer.GetMapped(), &instance, sizeof(instance));

		instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
		instances.data.deviceAddress = instanceBuffer.GetBufferAddress();

		asGeom.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		asGeom.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
		asGeom.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
		asGeom.geometry.instances = instances;

		offset.firstVertex = 0;
		offset.primitiveCount = 1;
		offset.primitiveOffset = 0;
		offset.transformOffset = 0;

		geometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		geometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
		geometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		geometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;

		geometryInfo.pGeometries = &asGeom;
		geometryInfo.geometryCount = 1;





		VK_FUNCTION(vkCreateAccelerationStructureKHR, device->GetDevice());
		VK_FUNCTION(vkGetAccelerationStructureBuildSizesKHR, device->GetDevice());

		uint32_t count = 1;

		VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
		sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

		vkGetAccelerationStructureBuildSizesKHR(device->GetDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &geometryInfo, &count, &sizeInfo);

		RTT_LOG("[ VULKAN ] [ TLAS ] Build Sizes");
		RTT_LOG("[ VULKAN ] [ TLAS ] Acceleration structure size: "+std::to_string(sizeInfo.accelerationStructureSize));
		RTT_LOG("[ VULKAN ] [ TLAS ] Scratch size: " + std::to_string(sizeInfo.buildScratchSize));

		scratchBuffer = Buffer(device, sizeInfo.buildScratchSize,
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
		);
		scratchBuffer.Create();
		geometryInfo.scratchData.deviceAddress = scratchBuffer.GetBufferAddress();

		acBuffer = Buffer(device, sizeInfo.accelerationStructureSize,
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR
		);
		acBuffer.Create();

		VkAccelerationStructureCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		createInfo.buffer = acBuffer.GetBuffer();
		createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;

		VK_CREATE_VALIDATION(vkCreateAccelerationStructureKHR(device->GetDevice(), &createInfo, nullptr, &tlas));

		geometryInfo.dstAccelerationStructure = tlas;

		
	}
	void TLAS::Destroy()
	{
		VK_FUNCTION(vkDestroyAccelerationStructureKHR, device->GetDevice());

		vkDestroyAccelerationStructureKHR(device->GetDevice(), tlas, nullptr);

		instanceBuffer.Destroy();
		acBuffer.Destroy();
		scratchBuffer.Destroy();
	}
	void TLAS::Build()
	{
		cmdPool = CommandPool(device);
		cmdPool.Create();
		cmd = CommandBuffer(device, &cmdPool);
		cmd.Create();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(cmd.GetBuffer(), &beginInfo);
		VK_FUNCTION(vkCmdBuildAccelerationStructuresKHR, device->GetDevice());
		VkAccelerationStructureBuildRangeInfoKHR        buildOffsetInfo{ 1, 0, 0, 0 };
		const VkAccelerationStructureBuildRangeInfoKHR* pBuildOffsetInfo = &buildOffsetInfo;
		vkCmdBuildAccelerationStructuresKHR(cmd.GetBuffer(), 1, &geometryInfo, &pBuildOffsetInfo);
		vkEndCommandBuffer(cmd.GetBuffer());

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = cmd.GetBufferP();

		vkQueueSubmit(device->GetGraphicsQueue(), 1, &submitInfo, nullptr);
		vkQueueWaitIdle(device->GetGraphicsQueue());

		cmd.Destroy();
		cmdPool.Destroy();
	}
	VkAccelerationStructureKHR* TLAS::GetAccelerationStructure()
	{

		return &tlas;
	}
}
