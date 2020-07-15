#include "pch.h"
#pragma hdrstop
#if !defined(QUADRIC_GL)
#include "mesh.h"
#include "magma.h"

namespace quadric
{
    class Mesh : public IMesh
    {
    public:
        explicit Mesh(uint32_t numVertices, uint32_t numFaces, std::shared_ptr<magma::CommandBuffer> copyCmd):
            vertexBufferSize(numVertices * sizeof(Vertex)),
            indexBufferSize(numFaces * sizeof(Face)),
            stagingBuffer(std::make_shared<magma::SrcTransferBuffer>(copyCmd->getDevice(), vertexBufferSize + indexBufferSize)),
            copyCmd(std::move(copyCmd))
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
            vertexBuffer = std::make_shared<magma::VertexBuffer>(copyCmd, stagingBuffer,
                vertexBufferSize, 0);
            indexBuffer = std::make_shared<magma::IndexBuffer>(std::move(copyCmd), std::move(stagingBuffer),
                VK_INDEX_TYPE_UINT16, indexBufferSize, vertexBufferSize);
        }

        void bind(std::shared_ptr<magma::CommandBuffer> cmdBuffer) const noexcept override
        {
            cmdBuffer->bindVertexBuffer(0, vertexBuffer);
            cmdBuffer->bindIndexBuffer(indexBuffer);
        }

        void draw(std::shared_ptr<magma::CommandBuffer> cmdBuffer, uint32_t indexCount, int32_t baseVertex) const noexcept override
        {
            cmdBuffer->drawIndexed(indexCount, 0, baseVertex);
        }

    private:
        const VkDeviceSize vertexBufferSize;
        const VkDeviceSize indexBufferSize;
        std::shared_ptr<magma::VertexBuffer> vertexBuffer;
        std::shared_ptr<magma::IndexBuffer> indexBuffer;
        std::shared_ptr<magma::SrcTransferBuffer> stagingBuffer;
        std::shared_ptr<magma::CommandBuffer> copyCmd;
        void *mapData;
    };

    std::unique_ptr<IMesh> newMesh(uint32_t numVertices, uint32_t numFaces, CommandBuffer copyCmd)
    {
        return std::make_unique<Mesh>(numVertices, numFaces, std::move(copyCmd));
    }
}

namespace magma
{
    namespace specialization
    {
        template<> struct VertexAttribute<rapid::float3> : public AttributeFormat<VK_FORMAT_R32G32B32_SFLOAT> {};
        template<> struct VertexAttribute<quadric::TexCoord> : public AttributeFormat<VK_FORMAT_R32G32_SFLOAT> {};
    }
}
#endif // !QUADRIC_GL
