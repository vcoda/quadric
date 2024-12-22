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
        explicit Mesh(uint32_t numVertices, uint32_t numFaces, std::shared_ptr<magma::CommandBuffer> copyCmd, std::shared_ptr<magma::Allocator> allocator):
            vertexBufferSize(numVertices * sizeof(Vertex)),
            indexBufferSize(numFaces * sizeof(Face)),
            stagingBuffer(std::make_shared<magma::SrcTransferBuffer>(copyCmd->getDevice(), vertexBufferSize + indexBufferSize, nullptr, allocator)),
            copyCmd(std::move(copyCmd)),
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

        const BoundingBox& getBoundingBox() const noexcept override
        {
            return bbox;
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
            computeBoundingBox();
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

        void bind(const std::shared_ptr<magma::CommandBuffer>& cmdBuffer) const noexcept override
        {
            cmdBuffer->bindVertexBuffer(0, vertexBuffer);
            cmdBuffer->bindIndexBuffer(indexBuffer);
        }

        void draw(const std::shared_ptr<magma::CommandBuffer>& cmdBuffer, uint32_t indexCount, int32_t baseVertex) const noexcept override
        {
            cmdBuffer->drawIndexed(indexCount, 0, baseVertex);
        }

        void drawInstanced(const std::shared_ptr<magma::CommandBuffer>& cmdBuffer, uint32_t indexCount, uint32_t instanceCount, int32_t baseVertex) const noexcept override
        {
            cmdBuffer->drawIndexedInstanced(indexCount, instanceCount, 0, baseVertex, 0);
        }

    private:
        void computeBoundingBox() noexcept
        {
            constexpr float min = std::numeric_limits<float>::max();
            constexpr float max = std::numeric_limits<float>::min();
            bbox.min = rapid::float3(min, min, min);
            bbox.max = rapid::float3(max, max, max);
            const Vertex *verts = (const Vertex *)stagingBuffer->getMemory()->getMapPointer();
            const uint64_t numVertices = vertexBufferSize / sizeof(Vertex);
            for (uint64_t i = 0; i < numVertices; ++i)
            {
                bbox.min = rapid::min3(bbox.min, verts[i].pos);
                bbox.max = rapid::max3(bbox.max, verts[i].pos);
            }
        }

        const VkDeviceSize vertexBufferSize;
        const VkDeviceSize indexBufferSize;
        std::shared_ptr<magma::VertexBuffer> vertexBuffer;
        std::shared_ptr<magma::IndexBuffer> indexBuffer;
        std::shared_ptr<magma::SrcTransferBuffer> stagingBuffer;
        std::shared_ptr<magma::CommandBuffer> copyCmd;
        std::shared_ptr<magma::Allocator> allocator;
        BoundingBox bbox;
        void *mapData;
    };

    std::unique_ptr<IMesh> newMesh(uint32_t numVertices, uint32_t numFaces, const std::shared_ptr<magma::CommandBuffer>& copyCmd, Allocator allocator)
    {
        return std::make_unique<Mesh>(numVertices, numFaces, copyCmd, std::move(allocator));
    }
}

MAGMA_SPECIALIZE_VERTEX_ATTRIBUTE(rapid::float3, VK_FORMAT_R32G32B32_SFLOAT);
MAGMA_SPECIALIZE_VERTEX_ATTRIBUTE(quadric::TexCoord, VK_FORMAT_R32G32_SFLOAT);

#endif // !QUADRIC_GL
