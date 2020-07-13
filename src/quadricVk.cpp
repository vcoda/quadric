#include "pch.h"
#pragma hdrstop
#ifdef QUADRIC_VULKAN
#include "quadric.h"

using namespace quadric;

Quadric::Quadric(uint16_t numVertices, uint32_t numFaces, backend::Device device):
    numVertices(numVertices),
    numFaces((uint16_t)numFaces),
    device(device)
{
    if (numFaces > std::numeric_limits<uint16_t>::max())
        throw std::length_error("face count exceed maximum unsigned short value");
    VkBufferCreateInfo bufferInfo = {
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        nullptr,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        numVertices * sizeof(Vertex),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_SHARING_MODE_EXCLUSIVE,
        0, nullptr,
    };
    VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, &vertices);
    bufferInfo.size = numFaces * sizeof(Face);
    result = vkCreateBuffer(device, &bufferInfo, nullptr, &indices);
    const VkMemoryAllocateInfo allocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        numVertices * sizeof(Vertex) + numFaces * sizeof(Face),
        0 // memoryTypeIndex
    };
    result = vkAllocateMemory(device, &allocInfo, nullptr, &memory);
    result = vkBindBufferMemory(device, vertices, memory, 0);
    result = vkBindBufferMemory(device, indices, memory, numVertices * sizeof(Vertex));
}

Quadric::~Quadric()
{
    vkFreeMemory(memory);
    vkDestroyBuffer(device, vertexBuffer, nullptr);
    vkDestroyBuffer(device, indexBuffer, nullptr);
}

const backend::VertexInputState& Quadric::getVertexInput() const noexcept
{
    constexpr VkVertexInputBindingDescription vertexBindingDescription = {
        0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX
    };
    constexpr VkVertexInputAttributeDescription vertexAttributeDescriptions[3] = {
        {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos)},
        {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)},
        {2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, tex)}
    };
    static const VkPipelineVertexInputStateCreateInfo vertexInput = {
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        nullptr, 0,
        1, &vertexBindingDescription,
        3, vertexAttributeDescriptions
    };
    return vertexInput;
}

void Quadric::draw(backend::CommandBuffer cmdBuffer) const noexcept
{
    constexpr VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexBuffer, &offset);
    vkCmdBindIndexBuffer(cmdBuffer, indexBuffer, offset, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(cmdBuffer,
        numFaces * 3, // indexCount
        1, // instanceCount
        0, 0, 0);
}

void Quadric::upload(backend::CommandBuffer cmdBuffer)
{
    const VkBufferCopy copyVertices = {0, 0, numVertices * sizeof(Vertex)};
    const VkBufferCopy copyIndices = {0, 0, numFaces * sizeof(Face)};
    constexpr VkCommandBufferBeginInfo beginInfo = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        nullptr, 0, nullptr
    };
    vkBeginCommandBuffer(cmdBuffer, &beginInfo);
    vkCmdCopyBuffer(cmdBuffer, vertices, vertexBuffer, 1, &copyVertices);
    vkCmdCopyBuffer(cmdBuffer, indices, indexBuffer, 1, &copyIndices);
    vkEndCommandBuffer(cmdBuffer);
}
#endif // QUADRIC_VULKAN
