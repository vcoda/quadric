#pragma once
#include "mesh.h"

namespace quadric
{
    class Quadric
    {
        Quadric(const Quadric&) = delete;
        Quadric& operator=(const Quadric&) = delete;

    public:
        virtual ~Quadric();
        uint16_t getNumVertices() const noexcept { return numVertices; }
        uint16_t getNumFaces() const noexcept { return numFaces; }
        const VertexInput& getVertexInput()  const noexcept;
        const BoundingBox& getBoundingBox() const noexcept;
        virtual void draw(const CommandBuffer& cmdBuffer) const noexcept;
        virtual void drawInstanced(const CommandBuffer& cmdBuffer, uint32_t instanceCount) const noexcept;

    protected:
        Quadric(uint16_t numVertices, uint32_t numFaces,
            const CommandBuffer& cmdBuffer, Allocator allocator);

        const uint16_t numVertices;
        const uint16_t numFaces;
        std::unique_ptr<IMesh> mesh;
    };
}
