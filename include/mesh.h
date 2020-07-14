#pragma once
#include "common.h"

#if !defined(QUADRIC_GL)
namespace magma
{
    class CommandBuffer;
    class VertexInputState;
}
#endif

namespace quadric
{
#ifdef QUADRIC_GL
    typedef void* CommandBuffer;
    typedef uint32_t VertexInput;
#else
    typedef std::shared_ptr<magma::CommandBuffer> CommandBuffer;
    typedef magma::VertexInputState VertexInput;
#endif

    class IMesh
    {
    public:
        virtual ~IMesh() = default;
        virtual const VertexInput& getVertexInput() const noexcept = 0;
        virtual Vertex *mapVertices() = 0;
        virtual Face *mapIndices() = 0;
        virtual void unmap() = 0;
        virtual void bind(CommandBuffer cmdBuffer) const noexcept = 0;
        virtual void draw(CommandBuffer cmdBuffer, uint32_t indexCount, int32_t baseVertex) const noexcept = 0;
    };

    std::unique_ptr<IMesh> newMesh(uint32_t numVertices, uint32_t numFaces, CommandBuffer cmdBuffer);
}
