#include "TLAS.h"

namespace rttvk {
	TLAS::TLAS(LogicalDevice* device, BLAS* blas)
	{
		this->device = device;	
		this->blas = blas;
	}
	void TLAS::Create()
	{
		DEVICE_FUNCTION(device, vkCreateAccelerationStructureKHR);
		DEVICE_FUNCTION(device, vkGetAccelerationStructureBuildSizesKHR);

		float transformMatrix[12] = {
			1,0,0,0,
			0,1,0,0,
			0,0,1,0
		};

		VkAccelerationStructureInstanceKHR instance{};
		instance.transform = VkTransformMatrixKHR{ *transformMatrix }; // Transformation matrix for the instance
		instance.instanceCustomIndex = 0; // Custom instance index (optional)
		instance.mask = 0xFF; // Visibility mask (optional)
		instance.instanceShaderBindingTableRecordOffset = 0; // Offset to the first shader binding table record of the hit group for the instance (optional)
		instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR; // Flags for the instance (optional)
		instance.accelerationStructureReference = blas->GetAddress(); // Handle of the bottom-level acceleration structure (BLAS) to be instanced

		blasInstance = Buffer(device, 1 * sizeof(VkAccelerationStructureInstanceKHR), VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR|VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
		blasInstance.Create();
		blasInstance.SetData(&instance, sizeof(VkAccelerationStructureInstanceKHR));

		geometryTLAS.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		geometryTLAS.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
		geometryTLAS.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
		geometryTLAS.geometry.instances.arrayOfPointers = VK_FALSE;
		geometryTLAS.geometry.instances.data.deviceAddress = blasInstance.GetBufferAddress();

		buildInfoTLAS.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		buildInfoTLAS.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
		buildInfoTLAS.geometryCount = 1;
		buildInfoTLAS.pGeometries = &geometryTLAS;
		buildInfoTLAS.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;

		uint32_t size = 1;

		VkAccelerationStructureBuildSizesInfoKHR sizeInfo = {};
		sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
		vkGetAccelerationStructureBuildSizesKHR(device->GetDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_HOST_KHR, &buildInfoTLAS, &size, &sizeInfo);

		VkAccelerationStructureCreateInfoKHR tlasCreateInfo = {};
		tlasCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		tlasCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;

		tlasBuffer = Buffer(device, sizeInfo.accelerationStructureSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
		tlasBuffer.Create();
		scratchBuffer = Buffer(device, 4, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
		if (sizeInfo.buildScratchSize > 0) {
			scratchBuffer = Buffer(device, sizeInfo.buildScratchSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
		}

		RTT_LOG("accelerationStructureSize: " + std::to_string(sizeInfo.accelerationStructureSize));
		RTT_LOG("buildScratchSize: " + std::to_string(sizeInfo.buildScratchSize));

		scratchBuffer.Create();
		buildInfoTLAS.scratchData.deviceAddress = scratchBuffer.GetBufferAddress();

		tlasCreateInfo.size = sizeInfo.accelerationStructureSize;
		tlasCreateInfo.buffer = tlasBuffer.GetBuffer();


		vkCreateAccelerationStructureKHR(device->GetDevice(), &tlasCreateInfo, nullptr, &tlas);

		buildInfoTLAS.srcAccelerationStructure = tlas;
		buildInfoTLAS.dstAccelerationStructure = tlas;

		buildRangeInfo.primitiveCount = 2;
		buildRangeInfo.primitiveOffset = 9;
		buildRangeInfo.firstVertex = 0;
		buildRangeInfo.transformOffset = 0;
		
	}
	void TLAS::Destroy()
	{
	}
	void TLAS::Build(CommandBuffer* commandBuffer)
	{
		DEVICE_FUNCTION(device, vkCmdBuildAccelerationStructuresKHR);
		VkAccelerationStructureBuildRangeInfoKHR* buildRangeInfos[] = { &buildRangeInfo };
		vkCmdBuildAccelerationStructuresKHR(commandBuffer->GetBuffer(), 1, &buildInfoTLAS, buildRangeInfos);
	}
	VkAccelerationStructureKHR TLAS::GetAS()
	{
		return tlas;
	}
}

