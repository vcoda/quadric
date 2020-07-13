#include "pch.h"
#pragma hdrstop
#ifdef QUADRIC_OPENGL
#include "quadric.h"

using namespace quadric;

Quadric::Quadric(uint16_t numVertices, uint32_t numFaces, backend::Device /* nullptr */):
    numVertices(numVertices),
    numFaces((uint16_t)numFaces)
{
    if (numFaces > std::numeric_limits<uint16_t>::max())
        throw std::length_error("face count exceed maximum unsigned short value");
    glGenBuffers(1, &vertices);
    glBindBuffer(GL_COPY_READ_BUFFER, vertices);
    glBufferData(GL_COPY_READ_BUFFER, numVertices * sizeof(Vertex), nullptr, GL_STATIC_COPY);
    glGenBuffers(1, &indices);
    glBindBuffer(GL_COPY_READ_BUFFER, indices);
    glBufferData(GL_COPY_READ_BUFFER, numFaces * sizeof(Face), nullptr, GL_STATIC_COPY);
}

Quadric::~Quadric()
{
    const GLuint buffers[2] = {vertexBuffer, indexBuffer};
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(2, buffers);
    glDeleteVertexArrays(1, &vertexArray);
}

const backend::VertexInputState& Quadric::getVertexInput() const noexcept
{
    return vertexArray;
}

void Quadric::draw(backend::CommandBuffer cmdBuffer) const noexcept
{
    glBindVertexArray(getVertexInput());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_SHORT, (const void *)0);
}

Vertex *Quadric::mapVertices()
{
    glBindBuffer(GL_COPY_READ_BUFFER, vertices);
    return (Vertex *)glMapBuffer(GL_COPY_READ_BUFFER, GL_WRITE_ONLY);
}

Face *Quadric::mapIndices()
{
    glBindBuffer(GL_COPY_READ_BUFFER, indices);
    return (Face *)glMapBuffer(GL_COPY_READ_BUFFER, GL_WRITE_ONLY);
}

void Quadric::upload(backend::CommandBuffer cmdBuffer)
{   // Unmap/Copy
    glBindBuffer(GL_COPY_READ_BUFFER, vertices);
    glUnmapBuffer(GL_COPY_READ_BUFFER);
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_COPY_WRITE_BUFFER, vertexBuffer);
    glBufferData(GL_COPY_WRITE_BUFFER, numVertices * sizeof(Vertex), nullptr, GL_STATIC_DRAW);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, numVertices * sizeof(Vertex));
    glBindBuffer(GL_COPY_READ_BUFFER, indices);
    glUnmapBuffer(GL_COPY_READ_BUFFER);
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_COPY_WRITE_BUFFER, indexBuffer);
    glBufferData(GL_COPY_WRITE_BUFFER, numFaces * sizeof(Face), nullptr, GL_STATIC_DRAW);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, numFaces * sizeof(Face));
    // Free
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glDeleteBuffers(1, &vertices);
    glDeleteBuffers(1, &indices);
    vertices = indices = 0;
    // Generate VAO
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    constexpr GLsizei stride = sizeof(Vertex);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void *)offsetof(Vertex, pos));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (const void *)offsetof(Vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (const void *)offsetof(Vertex, tex));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}
#endif // QUADRIC_OPENGL
