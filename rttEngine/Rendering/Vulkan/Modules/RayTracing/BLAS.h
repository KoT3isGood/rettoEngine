#pragma once
#include "..\LogicalDevice.h";
#include "..\CommandBuffer.h";
#include "..\Buffer.h";

namespace rttvk {
	class BLAS : public Module {
	public:
		BLAS(LogicalDevice* device);
		virtual void Create() override;
		virtual void Destroy() override;
		void Build(CommandBuffer* commandBuffer);
		uint64_t GetAddress();
	private:
		LogicalDevice* device;

		Buffer vertexBuffer;
		Buffer indexBuffer;

		Buffer blasBuffer;
		Buffer scratchBuffer;

		VkAccelerationStructureKHR blas;
		VkAccelerationStructureGeometryKHR geometryBLAS = {};
		VkAccelerationStructureBuildGeometryInfoKHR buildInfoBLAS = {};
		VkAccelerationStructureBuildRangeInfoKHR buildRangeInfo = {};
	};
}
