#pragma once
#include "..\LogicalDevice.h";
#include "..\CommandBuffer.h";
#include "..\Buffer.h";
#include "BLAS.h"

namespace rttvk {
	class TLAS : public Module {
	public:
		TLAS(LogicalDevice* device, Buffer* buffer, BLAS* blas);
		virtual void Create() override;
		virtual void Destroy() override;
		void Build(CommandBuffer* commandBuffer);
		VkAccelerationStructureKHR GetAS();
	private:
		LogicalDevice* device;
		Buffer* buffer;

		VkAccelerationStructureKHR structure;

		Buffer instancesBuffer;
		Buffer scratchBuffer;

		VkAccelerationStructureBuildGeometryInfoKHR geometryInfo{};
		VkAccelerationStructureBuildRangeInfoKHR buildRange{};
		VkAccelerationStructureGeometryKHR geometry{};
		VkAccelerationStructureGeometryDataKHR geometryData{};
		BLAS* blas;
	};
}
