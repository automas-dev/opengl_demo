#pragma once

#include <GL/glew.h>
// gl.h after glew.h, clang-format don't sort
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Texture.hpp"


class RenderBuffer {
    GLuint buffer;
    GLenum target;
    GLenum internal;

public:
    RenderBuffer(int width, int height, GLenum target, GLenum internal)
        : target(target), internal(internal) {
        glGenRenderbuffers(1, &buffer);
        resize(width, height);
    }

    RenderBuffer(RenderBuffer && other) {
        buffer = other.buffer;
        other.buffer = 0;
    }

    RenderBuffer(const RenderBuffer & other) = delete;
    RenderBuffer & operator=(const RenderBuffer & other) = delete;
    RenderBuffer & operator=(RenderBuffer && other) = delete;

    ~RenderBuffer() {
        if (buffer)
            glDeleteRenderbuffers(1, &buffer);
    }

    GLuint getBufferId() const {
        return buffer;
    }

    void resize(int width, int height) const {
        bind();
        glRenderbufferStorage(target, internal, width, height);
        unbind();
    }

    void bind() const {
        glBindRenderbuffer(GL_RENDERBUFFER, buffer);
    }

    void unbind() const {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
};

class FrameBuffer {
public:
    struct Attachment {};

private:
    GLuint buffer;

public:
    FrameBuffer() {
        glGenFramebuffers(1, &buffer);
    }

    FrameBuffer(FrameBuffer && other) {
        buffer = other.buffer;
        other.buffer = 0;
    }

    FrameBuffer(const FrameBuffer &) = delete;
    FrameBuffer & operator=(const FrameBuffer &) = delete;
    FrameBuffer & operator=(FrameBuffer &&) = delete;

    ~FrameBuffer() {
        if (buffer)
            glDeleteFramebuffers(1, &buffer);
    }

    GLuint getBufferId() const {
        return buffer;
    }

    void bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, buffer);
    }

    void unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};
