#pragma once
#include "..\LogicalDevice.h";
#include "..\Buffer.h"
#include "..\CommandBuffer.h";
#include "BLAS.h";
#include "Objects\Mesh.h"
namespace rttvk {
	class TLAS : public Module {
	public:
		TLAS(LogicalDevice* device, std::vector<Mesh>* meshes, std::vector<BLAS*>* blases);
		virtual void Create() override;
		virtual void Destroy() override;
		void Build();
		void Update();
		VkAccelerationStructureKHR* GetAccelerationStructure();
		
		
	private:
		std::vector<BLAS*>* blases;
		std::vector<Mesh>* meshes;
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
