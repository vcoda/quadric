#include "pch.h"
#pragma hdrstop
#if !defined(QUADRIC_GL)
#include "mesh.h"
#undef align
#include "magma.h"

namespace quadric
{
    class Mesh : public IMesh
    {
    public:
        explicit Mesh(uint32_t numVertices, uint32_t numFaces, const std::unique_ptr<magma::CommandBuffer>& copyCmd, std::shared_ptr<magma::Allocator> allocator):
            vertexBufferSize(numVertices * sizeof(Vertex)),
            indexBufferSize(numFaces * sizeof(Face)),
            stagingBuffer(std::make_shared<magma::SrcTransferBuffer>(copyCmd->getDevice(), vertexBufferSize + indexBufferSize, nullptr, allocator)),
            copyCmd(copyCmd),
            allocator(std::move(allocator)),
            mapData(nullptr)
        {}

        const VertexInput& getVertexInput() const noexcept override
        {
            static magma::VertexInputStructure<Vertex> vertexInput(0, {
                {0, &Vertex::pos},
                {1, &Vertex::normal},
                {2, &Vertex::tex}});
            return vertexInput;
        }

        Vertex *mapVertices() override
        {
            if (!mapData)
                mapData = stagingBuffer->getMemory()->map();
            return (Vertex *)mapData;
        }

        Face *mapIndices() override
        {
            if (!mapData)
                mapData = stagingBuffer->getMemory()->map();
            return (Face *)(((char *)mapData) + vertexBufferSize);
        }

        void unmap() override
        {
            stagingBuffer->getMemory()->unmap();
            mapData = nullptr;
            copyCmd->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
            {
                vertexBuffer = std::make_shared<magma::VertexBuffer>(copyCmd, stagingBuffer,
                    allocator, vertexBufferSize, 0);
                indexBuffer = std::make_shared<magma::IndexBuffer>(copyCmd, VK_INDEX_TYPE_UINT16, stagingBuffer,
                    std::move(allocator), indexBufferSize, vertexBufferSize);
            }
            copyCmd->end();
            magma::finish(copyCmd);
            stagingBuffer.reset(); // Don't need anymore
        }

        void bind(const std::unique_ptr<magma::CommandBuffer>& cmdBuffer) const noexcept override
        {
            cmdBuffer->bindVertexBuffer(0, vertexBuffer);
            cmdBuffer->bindIndexBuffer(indexBuffer);
        }

        void draw(const std::unique_ptr<magma::CommandBuffer>& cmdBuffer, uint32_t indexCount, int32_t baseVertex) const noexcept override
        {
            cmdBuffer->drawIndexed(indexCount, 0, baseVertex);
        }

    private:
        const VkDeviceSize vertexBufferSize;
        const VkDeviceSize indexBufferSize;
        std::shared_ptr<magma::VertexBuffer> vertexBuffer;
        std::shared_ptr<magma::IndexBuffer> indexBuffer;
        std::shared_ptr<magma::SrcTransferBuffer> stagingBuffer;
        const std::unique_ptr<magma::CommandBuffer>& copyCmd;
        std::shared_ptr<magma::Allocator> allocator;
        void *mapData;
    };

    std::unique_ptr<IMesh> newMesh(uint32_t numVertices, uint32_t numFaces, const CommandBuffer& copyCmd, Allocator allocator)
    {
        return std::make_unique<Mesh>(numVertices, numFaces, copyCmd, std::move(allocator));
    }
}

MAGMA_SPECIALIZE_VERTEX_ATTRIBUTE(rapid::float3, VK_FORMAT_R32G32B32_SFLOAT);
MAGMA_SPECIALIZE_VERTEX_ATTRIBUTE(quadric::TexCoord, VK_FORMAT_R32G32_SFLOAT);

#endif // !QUADRIC_GL
