#include "pch.h"
#pragma hdrstop
#ifdef QUADRIC_GL
#ifdef _WIN32
#   ifndef NOMINMAX
#   define NOMINMAX
#   endif
#endif
#define GL_GLEXT_PROTOTYPES
#include <glcorearb.h>

#include "mesh.h"

namespace quadric
{
    class MeshGL : public IMesh
    {
    public:
        explicit MeshGL(uint32_t numVertices, uint32_t numFaces):
            vertexBufferSize(numVertices * sizeof(Vertex)),
            indexBufferSize(numFaces * sizeof(Face)),
            vertexBuffer(0),
            indexBuffer(0),
            stagingBuffer(0),
            vertexArray(0),
            mapData(nullptr)
        {
            glGenBuffers(1, &stagingBuffer);
            glBindBuffer(GL_COPY_READ_BUFFER, stagingBuffer);
            glBufferData(GL_COPY_READ_BUFFER, vertexBufferSize + indexBufferSize, nullptr, GL_STATIC_COPY);
        }

        ~MeshGL()
        {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            const GLuint buffers[2] = {vertexBuffer, indexBuffer};
            glDeleteBuffers(2, buffers);
            glDeleteVertexArrays(1, &vertexArray);
        }

        const VertexInput& getVertexInput() const noexcept override
        {
            if (!vertexArray)
            {
                glGenVertexArrays(1, &vertexArray);
                glBindVertexArray(vertexArray);
                glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, pos));
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, normal));
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, tex));
                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);
                glEnableVertexAttribArray(2);
                glBindVertexArray(0);
            }
            return vertexArray;
        }

        Vertex *mapVertices() override
        {
            if (!mapData)
            {
                glBindBuffer(GL_COPY_READ_BUFFER, stagingBuffer);
                mapData = glMapBuffer(GL_COPY_READ_BUFFER, GL_WRITE_ONLY);
            }
            return (Vertex *)mapData;
        }

        Face *mapIndices() override
        {
            if (!mapData)
            {
                glBindBuffer(GL_COPY_READ_BUFFER, stagingBuffer);
                mapData = glMapBuffer(GL_COPY_READ_BUFFER, GL_WRITE_ONLY);
            }
            return (Face *)(((char *)mapData) + vertexBufferSize);
        }

        void unmap() override
        {
            glBindBuffer(GL_COPY_READ_BUFFER, stagingBuffer);
            glUnmapBuffer(GL_COPY_READ_BUFFER);
            mapData = nullptr;
            // Copy vertex data
            glGenBuffers(1, &vertexBuffer);
            glBindBuffer(GL_COPY_WRITE_BUFFER, vertexBuffer);
            glBufferData(GL_COPY_WRITE_BUFFER, vertexBufferSize, nullptr, GL_STATIC_DRAW);
            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
                0, 0, vertexBufferSize);
            // Copy index data
            glGenBuffers(1, &indexBuffer);
            glBindBuffer(GL_COPY_WRITE_BUFFER, indexBuffer);
            glBufferData(GL_COPY_WRITE_BUFFER, indexBufferSize, nullptr, GL_STATIC_DRAW);
            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
                vertexBufferSize, 0, indexBufferSize);
            // Free storage buffer
            glBindBuffer(GL_COPY_READ_BUFFER, 0);
            glDeleteBuffers(1, &stagingBuffer);
            stagingBuffer = 0;
        }

        virtual void bind(void *) const noexcept override
        {
            glBindVertexArray(getVertexInput());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        }

        virtual void draw(void *, uint32_t indexCount, int32_t baseVertex) const noexcept override
        {
            glDrawElementsBaseVertex(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, (const void *)0, baseVertex);
        }

    private:
        const GLsizeiptr vertexBufferSize;
        const GLsizeiptr indexBufferSize;
        GLuint vertexBuffer;
        GLuint indexBuffer;
        GLuint stagingBuffer;
        mutable GLuint vertexArray;
        void *mapData;
    };

    std::unique_ptr<IMesh> newMesh(uint32_t numVertices, uint32_t numFaces, CommandBuffer /* nullptr */)
    {
        return std::make_unique<MeshGL>(numVertices, numFaces);
    }
}
#endif // QUADRIC_GL
