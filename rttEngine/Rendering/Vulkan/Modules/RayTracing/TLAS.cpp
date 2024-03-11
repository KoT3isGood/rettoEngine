#include "TLAS.h"

namespace rttvk {
	TLAS::TLAS(LogicalDevice* device, Buffer* buffer, BLAS* blas)
	{
		this->device = device;
		this->buffer = buffer;
		this->blas = blas;
	}
	void TLAS::Create()
	{
		DEVICE_FUNCTION(device, vkCreateAccelerationStructureKHR);
		scratchBuffer = Buffer(device, 160, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
		scratchBuffer.Create();
		


		VkAccelerationStructureCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		createInfo.size = 256;
		createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
		createInfo.buffer = buffer->GetBuffer();
		VK_CREATE_VALIDATION(vkCreateAccelerationStructureKHR(device->GetDevice(), &createInfo, nullptr, &structure), VkAccelerationStructureKHR);

		VkAccelerationStructureInstanceKHR instance{};
		float matrix[12] = {
			1,0,0,0,
			0,1,0,0,
			0,0,1,0
		};
		instance.transform = VkTransformMatrixKHR{ *matrix };
		instance.instanceCustomIndex = 0;
		instance.mask = 0xff;
		instance.instanceShaderBindingTableRecordOffset = 0;
		instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
		instance.accelerationStructureReference = blas->GetAddress();

		instancesBuffer = Buffer(device, 160, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
		instancesBuffer.Create();
		memcpy(instancesBuffer.GetMapped(), &instance, 160);


		VkAccelerationStructureGeometryInstancesDataKHR instances{};
		instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
		instances.arrayOfPointers = VK_FALSE;
		instances.data.deviceAddress = instancesBuffer.GetBufferAddress();

		geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
		geometry.geometry.instances = instances;
		//geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;

		geometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		geometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
		geometryInfo.flags = 0;
		geometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		geometryInfo.srcAccelerationStructure = VK_NULL_HANDLE;
		geometryInfo.dstAccelerationStructure = structure;
		geometryInfo.geometryCount = 1;
		geometryInfo.pGeometries = &geometry;
		geometryInfo.scratchData.deviceAddress = scratchBuffer.GetBufferAddress();

		buildRange.primitiveCount = 1;
		buildRange.primitiveOffset = 0;
		buildRange.firstVertex = 0;
		buildRange.transformOffset = 0;
	}
	void TLAS::Destroy()
	{
	}
	void TLAS::Build(CommandBuffer* commandBuffer)
	{
		DEVICE_FUNCTION(device, vkCmdBuildAccelerationStructuresKHR);
		std::vector<VkAccelerationStructureBuildRangeInfoKHR*> buildRanges = { &buildRange };
		vkCmdBuildAccelerationStructuresKHR(commandBuffer->GetBuffer(), 1, &geometryInfo, buildRanges.data());
	}
	VkAccelerationStructureKHR TLAS::GetAS()
	{
		return structure;
	}
}

