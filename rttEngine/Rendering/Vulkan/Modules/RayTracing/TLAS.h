#pragma once
#include "..\LogicalDevice.h";
#include "..\Buffer.h"
#include "..\CommandBuffer.h";
#include "BLAS.h";
#include "Objects\Mesh.h"
struct ObjectData {
	uint64_t verticesBufferAddr;
	uint64_t indexBufferAddr;
	uint64_t uvBufferAddr;
	uint64_t uvIndexBufferAddr;
	uint64_t materialBufferAddr;
	uint64_t materialIndexBufferAddr;
};
namespace rttvk {
	class TLAS : public Module {
	public:
		TLAS(LogicalDevice* device, std::vector<Mesh>* meshes, std::vector<BLAS>* blases);
		virtual void Create() override;
		virtual void Destroy() override;
		void Build();
		void Recreate();
		void Update(CommandBuffer* cmd);
		VkAccelerationStructureKHR* GetAccelerationStructure();
		

		Buffer blasesDataBuffer;
	private:
		uint32_t acBufSize = 0;
		std::vector<BLAS>* blases;
		std::vector<Mesh>* meshes;
		std::vector<ObjectData> meshesOD;
		std::vector<VkAccelerationStructureInstanceKHR> instancesArray = {};

		LogicalDevice* device;

		Buffer instanceBuffer;
		Buffer acBuffer;
		Buffer scratchBuffer;

		VkAccelerationStructureInstanceKHR instance{};
		VkAccelerationStructureGeometryInstancesDataKHR instances{};
		VkAccelerationStructureGeometryKHR asGeom{};
		VkAccelerationStructureBuildRangeInfoKHR offset;
		VkAccelerationStructureBuildGeometryInfoKHR geometryInfo{};
		VkAccelerationStructureKHR tlas;

		CommandPool cmdPool;
		CommandBuffer cmd;
	};
}
