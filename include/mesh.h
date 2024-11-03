#pragma once
#include <cstdint>
#if !defined(DIRECTX_MATH_VERSION)
// Include once
#include "rapid/rapid.h"
#endif

#if !defined(QUADRIC_GL)
namespace magma
{
    class CommandBuffer;
    class Allocator;
    class VertexInputState;
}
#endif

namespace quadric
{
    struct TexCoord
    {
        float s;
        float t;
    };

    struct Vertex
    {
        rapid::float3 pos;
        rapid::float3 normal;
        TexCoord tex;
    };

    struct Face
    {
        uint16_t v[3];
    };

    struct BoundingBox
    {
        rapid::float3 min;
        rapid::float3 max;
    };

#ifdef QUADRIC_GL
    typedef void* CommandBuffer;
    typedef void* Allocator;
    typedef uint32_t VertexInput;
#else
    typedef std::unique_ptr<magma::CommandBuffer> CommandBuffer;
    typedef std::shared_ptr<magma::Allocator> Allocator;
    typedef magma::VertexInputState VertexInput;
#endif

    class IMesh
    {
    public:
        virtual ~IMesh() = default;
        virtual const VertexInput& getVertexInput() const noexcept = 0;
        virtual const BoundingBox& getBoundingBox() const noexcept = 0;
        virtual Vertex *mapVertices() = 0;
        virtual Face *mapIndices() = 0;
        virtual void unmap() = 0;
        virtual void bind(const CommandBuffer& cmdBuffer) const noexcept = 0;
        virtual void draw(const CommandBuffer& cmdBuffer, uint32_t indexCount, int32_t baseVertex) const noexcept = 0;
    };

    std::unique_ptr<IMesh> newMesh(uint32_t numVertices, uint32_t numFaces, const CommandBuffer& cmdBuffer, Allocator allocator);
}
