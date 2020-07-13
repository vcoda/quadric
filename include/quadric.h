#pragma once
#include "common.h"

namespace quadric
{
    class Quadric
    {
    public:
        uint16_t getNumVertices() const { return numVertices; }
        uint16_t getNumFaces() const { return numFaces; }
        const magma::VertexInputState& getVertexInput()  const noexcept;
        virtual void draw(std::shared_ptr<magma::CommandBuffer> cmdBuffer) const noexcept;

    protected:
        Quadric(uint16_t numVertices, uint32_t numFaces,
            std::shared_ptr<magma::Device> device);
        void upload(std::shared_ptr<magma::CommandBuffer> cmdBuffer);

        const uint16_t numVertices;
        const uint16_t numFaces;
        std::shared_ptr<magma::SrcTransferBuffer> vertices;
        std::shared_ptr<magma::SrcTransferBuffer> indices;
        std::shared_ptr<magma::VertexBuffer> vertexBuffer;
        std::shared_ptr<magma::IndexBuffer> indexBuffer;
    };
}
