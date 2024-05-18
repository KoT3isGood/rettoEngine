#include "TLAS.h"
namespace rttvk {
	TLAS::TLAS(LogicalDevice* device, std::vector<Mesh>* meshes, std::vector<BLAS>* blases)
	{
		this->device = device;
		this->meshes = meshes;
		this->blases = blases;
	}
	void TLAS::Create()
	{
		
		if (meshes->size() == 0) {
			Mesh testMesh = Mesh();
			testMesh.pos[0] = 0;
			testMesh.pos[1] = 0;
			testMesh.pos[2] = 0;



			*meshes = { testMesh };
		}

		instanceBuffer = Buffer(device, sizeof(VkAccelerationStructureInstanceKHR)*meshes->size(),
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR
		);
		instanceBuffer.Create();


		for (int i = 0; i < blases->size(); i++) {
			meshesOD.push_back(ObjectData{(*blases)[i].vertexBuffer.GetBufferAddress(),(*blases)[i].indexBuffer.GetBufferAddress(),(*blases)[i].uvBuffer.GetBufferAddress(),(*blases)[i].uvIndexesBuffer.GetBufferAddress(),(*blases)[i].materialBuffer.GetBufferAddress(),(*blases)[i].materialIndexesBuffer.GetBufferAddress() });
		}
		blasesDataBuffer = Buffer(device, 48 * blases->size(),
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
		);
		blasesDataBuffer.Create();
		memcpy(blasesDataBuffer.GetMapped(), meshesOD.data(), 48 * blases->size());

		for (int i = 0; i < meshes->size(); i++) {
			instance.transform = VkTransformMatrixKHR{
			(*meshes)[i].rot[0],(*meshes)[i].rot[1],(*meshes)[i].rot[2],(*meshes)[i].pos[0],
			(*meshes)[i].rot[3],(*meshes)[i].rot[4],(*meshes)[i].rot[5],(*meshes)[i].pos[1],
			(*meshes)[i].rot[6],(*meshes)[i].rot[7],(*meshes)[i].rot[8],(*meshes)[i].pos[2]
			};
			
			instance.instanceCustomIndex = (*meshes)[i].instanceID;
			instance.accelerationStructureReference = (*blases)[(*meshes)[i].instanceID].GetAccelerationStructureAddress();
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

		geometryInfo.dstAccelerationStructure = VK_NULL_HANDLE;



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

		VkMemoryBarrier memoryBarrier = {};
		memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
		memoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
		memoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;

		vkCmdPipelineBarrier(cmd.GetBuffer(),
			VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
			VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
			0,
			1, &memoryBarrier,
			0, nullptr,
			0, nullptr);
		vkEndCommandBuffer(cmd.GetBuffer());

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = cmd.GetBufferP();

		vkQueueSubmit(device->GetGraphicsQueue(), 1, &submitInfo, nullptr);
		vkQueueWaitIdle(device->GetGraphicsQueue());
		// Destroy the command buffer
		vkFreeCommandBuffers(device->GetDevice(), cmdPool.GetPool(), 1, cmd.GetBufferP());

		cmd.Destroy();
		cmdPool.Destroy();
	}
	void TLAS::Recreate()
	{
		instancesArray = {};
		VK_FUNCTION(vkDestroyAccelerationStructureKHR, device->GetDevice());
		vkDestroyAccelerationStructureKHR(device->GetDevice(), tlas, nullptr);

		instanceBuffer.Destroy();
		instanceBuffer = Buffer(device, sizeof(VkAccelerationStructureInstanceKHR) * meshes->size(),
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
			instance.accelerationStructureReference = (*blases)[(*meshes)[i].instanceID].GetAccelerationStructureAddress();
			instance.flags = 0;
			instance.mask = 0xFF;
			instance.instanceShaderBindingTableRecordOffset = 0;

			instancesArray.push_back(instance);
		}
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
		geometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR;
		geometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;

		geometryInfo.pGeometries = &asGeom;
		geometryInfo.geometryCount = 1;

		memcpy((uint64_t*)instanceBuffer.GetMapped(), instancesArray.data(), sizeof(VkAccelerationStructureInstanceKHR)* meshes->size());

		uint32_t count = 1;

		VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
		sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
		geometryInfo.dstAccelerationStructure = nullptr;
		VK_FUNCTION(vkCreateAccelerationStructureKHR, device->GetDevice());
		VK_FUNCTION(vkGetAccelerationStructureBuildSizesKHR, device->GetDevice());
		vkGetAccelerationStructureBuildSizesKHR(device->GetDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &geometryInfo, &count, &sizeInfo);
		scratchBuffer.Destroy();
		scratchBuffer = Buffer(device, sizeInfo.buildScratchSize,
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
		);
		scratchBuffer.Create();
		geometryInfo.scratchData.deviceAddress = scratchBuffer.GetBufferAddress();
		acBuffer.Destroy();
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
	void TLAS::Update(CommandBuffer* cmd)
	{
		VK_FUNCTION(vkCmdBuildAccelerationStructuresKHR, device->GetDevice());
		const VkAccelerationStructureBuildRangeInfoKHR* pBuildOffsetInfo = &offset;
		
		vkCmdBuildAccelerationStructuresKHR(cmd->GetBuffer(), 1, &geometryInfo, &pBuildOffsetInfo);

		VkMemoryBarrier memoryBarrier = {};
		memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
		memoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
		memoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;

		vkCmdPipelineBarrier(cmd->GetBuffer(),
			VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
			VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
			0,
			1, &memoryBarrier,
			0, nullptr,
			0, nullptr);
	}
	VkAccelerationStructureKHR* TLAS::GetAccelerationStructure()
	{

		return &tlas;
	}
}
