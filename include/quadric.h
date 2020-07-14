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
        uint16_t getNumVertices() const { return numVertices; }
        uint16_t getNumFaces() const { return numFaces; }
        const VertexInput& getVertexInput()  const noexcept;
        virtual void draw(CommandBuffer cmdBuffer) const noexcept;

    protected:
        Quadric(uint16_t numVertices, uint32_t numFaces, CommandBuffer cmdBuffer);

        const uint16_t numVertices;
        const uint16_t numFaces;
        std::unique_ptr<IMesh> mesh;
    };
}
