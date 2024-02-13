#include "CommandBuffer.h"
namespace rttvk {
    CommandBuffer::CommandBuffer(LogicalDevice* device, CommandPool* pool)
    {
        this->pool = pool;
        this->device = device;
    }
    void CommandBuffer::Create()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = pool->GetPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;
        VK_CREATE_VALIDATION(vkAllocateCommandBuffers(device->GetDevice(), &allocInfo, &buffer), VkCommandBuffer);
    }
    VkCommandBuffer CommandBuffer::GetBuffer()
    {
        return buffer;
    }
}