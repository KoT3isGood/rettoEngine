#include "TLAS.h"
namespace rttvk {
	TLAS::TLAS(LogicalDevice* device, std::vector<Mesh>* meshes, std::vector<BLAS*>* blases)
	{
		this->device = device;
		this->meshes = meshes;
		this->blases = blases;
	}
	void TLAS::Create()
	{
		
		

		instanceBuffer = Buffer(device, sizeof(VkAccelerationStructureInstanceKHR)*meshes->size(),
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR
		);
		instanceBuffer.Create();

		for (int i = 0; i < meshes->size(); i++) {
			instance.transform = VkTransformMatrixKHR{
			(*meshes)[i].rot[0],(*meshes)[i].rot[1],(*meshes)[i].rot[2],(*meshes)[i].pos[0],
			(*meshes)[i].rot[3],(*meshes)[i].rot[4],(*meshes)[i].rot[5],(*meshes)[i].pos[1],
			(*meshes)[i].rot[6],(*meshes)[i].rot[7],(*meshes)[i].rot[8],(*meshes)[i].pos[2]
			};
			
			instance.instanceCustomIndex = (*meshes)[i].instanceID;
			instance.accelerationStructureReference = (*blases)[(*meshes)[i].instanceID]->GetAccelerationStructureAddress();
			instance.flags = 0;
			instance.mask = 0xFF;
			instance.instanceShaderBindingTableRecordOffset = 0;

			instancesArray.push_back(instance);
		}
		memcpy((uint64_t*)instanceBuffer.GetMapped(), instancesArray.data(), sizeof(VkAccelerationStructureInstanceKHR) * meshes->size());

		instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
		instances.data.deviceAddress = instanceBuffer.GetBufferAddress();

		asGeom.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		asGeom.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
		asGeom.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
		asGeom.geometry.instances = instances;

		offset.firstVertex = 0;
		offset.primitiveCount = meshes->size();
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
		const VkAccelerationStructureBuildRangeInfoKHR* pBuildOffsetInfo = &offset;
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
	void TLAS::Update()
	{
		VK_FUNCTION(vkCmdBuildAccelerationStructuresKHR, device->GetDevice());
		const VkAccelerationStructureBuildRangeInfoKHR* pBuildOffsetInfo = &offset;
		//vkCmdBuildAccelerationStructuresKHR(cmd.GetBuffer(), 1, &geometryInfo, &pBuildOffsetInfo);
	}
	VkAccelerationStructureKHR* TLAS::GetAccelerationStructure()
	{

		return &tlas;
	}
}
