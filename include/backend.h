#pragma once

#if !defined(QUADRIC_VULKAN) && !defined(QUADRIC_OPENGL) && !defined(QUADRIC_DIRECT3D)
namespace magma
{
    class Device;
    class VertexBuffer;
    class IndexBuffer;
    class SrcTransferBuffer;
    class CommandBuffer;
    class VertexInputState;
}
#endif // !QUADRIC_VULKAN && !QUADRIC_OPENGL && !QUADRIC_DIRECT3D

namespace quadric
{
    namespace backend
    {
#if defined(QUADRIC_VULKAN)
        typedef VkDevice Device;
        typedef VkDeviceMemory BufferMemory;
        typedef VkCommandBuffer CommandBuffer;
        typedef VkBuffer VertexBuffer;
        typedef VkBuffer IndexBuffer;
        typedef VkBuffer SrcTransferBuffer;
        typedef VkDevice Device;
        typedef VkPipelineVertexInputStateCreateInfo VertexInputState;
#elif defined(QUADRIC_OPENGL)
        typedef void* Device;
        typedef void* BufferMemory;
        typedef void* CommandBuffer;
        typedef GLuint VertexBuffer;
        typedef GLuint IndexBuffer;
        typedef GLuint SrcTransferBuffer;
        typedef GLuint VertexInputState; // VAO
#elif defined(QUADRIC_DIRECT3D)
        // TODO
#else
        typedef std::shared_ptr<magma::Device> Device;
        typedef std::shared_ptr<magma::DeviceMemory> BufferMemory;
        typedef std::shared_ptr<magma::CommandBuffer> CommandBuffer;
        typedef std::shared_ptr<magma::VertexBuffer> VertexBuffer;
        typedef std::shared_ptr<magma::IndexBuffer> IndexBuffer;
        typedef std::shared_ptr<magma::SrcTransferBuffer> SrcTransferBuffer;
        typedef magma::VertexInputState VertexInputState;
#endif // !QUADRIC_VULKAN && !QUADRIC_OPENGL && !QUADRIC_DIRECT3D
    }
}
