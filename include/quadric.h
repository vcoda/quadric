#pragma once
#include "common.h"

namespace quadric
{
    class Quadric
    {
    public:
        virtual ~Quadric();
        uint16_t getNumVertices() const { return numVertices; }
        uint16_t getNumFaces() const { return numFaces; }
        const backend::VertexInputState& getVertexInput()  const noexcept;
        virtual void draw(backend::CommandBuffer cmdBuffer) const noexcept;

    protected:
        Quadric(uint16_t numVertices, uint32_t numFaces, backend::Device device);
        Vertex *mapVertices();
        Face *mapIndices();
        void upload(backend::CommandBuffer cmdBuffer);

        const uint16_t numVertices;
        const uint16_t numFaces;
        backend::VertexBuffer vertexBuffer;
        backend::IndexBuffer indexBuffer;

    private:
        backend::Device device;
        backend::SrcTransferBuffer vertices;
        backend::SrcTransferBuffer indices;
        backend::VertexBuffer vertexArray;
        backend::BufferMemory memory;
        void *memData = nullptr;
    };
}
