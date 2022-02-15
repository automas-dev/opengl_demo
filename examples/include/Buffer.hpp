#pragma once

#include <GL/glew.h>
// gl.h after glew.h, clang-format don't sort
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <stdexcept>
#include <vector>
#include <iostream>

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
    Attribute attr;
    GLuint buffer;

public:
    Buffer(const Attribute & attr) : Buffer(GL_ARRAY_BUFFER, attr) {}

    Buffer(GLenum target, const Attribute & attr) : target(target), attr(attr) {
        glGenBuffers(1, &buffer);
    }

    Buffer(Buffer && other) {
        target = other.target;
        std::swap(attr, other.attr);
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

    const Attribute & getAttribute() const {
        return attr;
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
        attr.enable();
    }
};

class BufferArray {
    GLuint array;
    std::vector<Buffer> buffers;

public:
    BufferArray() {
        glGenVertexArrays(1, &array);
    }

    BufferArray(std::vector<Buffer> && buffers) : buffers(std::move(buffers)) {}

    BufferArray(const std::vector<Attribute> & attributes) : BufferArray() {
        for (auto & attr : attributes) {
            buffers.emplace_back(GL_ARRAY_BUFFER, attr);
        }
    }

    BufferArray(const BufferArray &) = delete;
    BufferArray(BufferArray &&) = delete;
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

    std::vector<Buffer> & getBuffers() {
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

    void drawArrays(GLenum mode, GLint first, GLsizei count) {
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
