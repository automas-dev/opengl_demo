#pragma once

#include <GL/glew.h>
// gl.h after glew.h, clang-format don't sort
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

struct Attribute {
    GLuint index;
    GLint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    const void * pointer;

    void enable() const {
        glVertexAttribPointer(index, size, type, normalized, stride, pointer);
        glEnableVertexAttribArray(index);
    }

    void disable() const {
        glDisableVertexAttribArray(index);
    }
};

class Buffer {
    GLenum target;
    GLuint buffer;

public:
    Buffer(GLenum target = GL_ARRAY_BUFFER) : target(target) {
        glGenBuffers(1, &buffer);
    }

    Buffer(Buffer && other) {
        target = other.target;
        buffer = other.buffer;
        other.buffer = 0;
    }

    Buffer(const Buffer &) = delete;
    Buffer & operator=(const Buffer &) = delete;
    Buffer & operator=(Buffer &&) = delete;

    ~Buffer() {
        if (buffer != 0)
            glDeleteBuffers(1, &buffer);
    }

    GLenum getTarget() const {
        return target;
    }

    GLuint getBufferId() const {
        return buffer;
    }

    void bind() const {
        glBindBuffer(target, buffer);
    }

    void unbind() const {
        glBindBuffer(target, 0);
    }

    void bufferData(GLsizeiptr size, const void * data, GLenum usage = GL_STATIC_DRAW) {
        bind();
        glBufferData(target, size, data, usage);
    }

    void bufferSubData(GLintptr offset, GLsizeiptr size, const void * data) {
        bind();
        glBufferSubData(target, offset, size, data);
    }
};

struct AttributedBuffer {
    Attribute attrib;
    Buffer buffer;

    AttributedBuffer(const Attribute & attrib, Buffer && buffer)
        : attrib(attrib), buffer(std::move(buffer)) {}

    AttributedBuffer(AttributedBuffer && other)
        : attrib(other.attrib), buffer(std::move(other.buffer)) {}

    AttributedBuffer(const AttributedBuffer &) = delete;
    AttributedBuffer & operator=(const AttributedBuffer &) = delete;
    AttributedBuffer & operator=(AttributedBuffer &&) = delete;

    inline void bufferData(GLsizeiptr size,
                           const void * data,
                           GLenum usage = GL_STATIC_DRAW) {
        buffer.bufferData(size, data, usage);
        attrib.enable();
    }

    inline void bufferSubData(GLintptr offset, GLsizeiptr size, const void * data) {
        buffer.bufferSubData(offset, size, data);
    }
};

class BufferArray {
    GLuint array;
    std::vector<AttributedBuffer> buffers;
    std::unique_ptr<Buffer> elementBuffer;

public:
    BufferArray() : elementBuffer(nullptr) {
        if (array)
            glGenVertexArrays(1, &array);
    }

    BufferArray(const std::vector<Attribute> & attributes) : BufferArray() {
        for (auto & attr : attributes) {
            Buffer buffer(GL_ARRAY_BUFFER);
            buffers.emplace_back(attr, std::move(buffer));
        }
    }

    BufferArray(BufferArray && other) {
        buffers = std::move(other.buffers);
        elementBuffer = std::move(other.elementBuffer);
        array = other.array;
        other.array = 0;
    }

    BufferArray(const BufferArray &) = delete;
    BufferArray & operator=(const BufferArray &) = delete;
    BufferArray & operator=(BufferArray &&) = delete;

    ~BufferArray() {
        glDeleteVertexArrays(1, &array);
    }

    GLuint getArrayId() const {
        return array;
    }

    std::size_t size() const {
        return buffers.size();
    }

    std::vector<AttributedBuffer> & getBuffers() {
        return buffers;
    }

    void bind() const {
        glBindVertexArray(array);
    }

    void unbind() const {
        glBindVertexArray(0);
    }

    void bufferData(size_t index,
                    GLsizeiptr size,
                    const void * data,
                    GLenum usage = GL_STATIC_DRAW) {
        buffers[index].bufferData(size, data, usage);
    }

    void bufferSubData(size_t index,
                       GLintptr offset,
                       GLsizeiptr size,
                       const void * data) {
        buffers[index].bufferSubData(offset, size, data);
    }

    void bufferElements(GLsizeiptr size,
                        const void * data,
                        GLenum usage = GL_STATIC_DRAW) {
        if (!elementBuffer)
            elementBuffer = std::make_unique<Buffer>(GL_ELEMENT_ARRAY_BUFFER);
        elementBuffer->bufferData(size, data, usage);
    }

    void drawArrays(GLenum mode, GLint first, GLsizei count) const {
        bind();
        glDrawArrays(mode, first, count);
    }

    void drawElements(GLenum mode,
                      GLsizei count,
                      GLenum type,
                      const void * indices) const {
        bind();
        glDrawElements(mode, count, type, indices);
    }
};
