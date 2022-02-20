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
    GLenum internal;

public:
    RenderBuffer(int width, int height, GLenum internal) : internal(internal) {
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
        glRenderbufferStorage(GL_RENDERBUFFER, internal, width, height);
    }

    void bind() const {
        glBindRenderbuffer(GL_RENDERBUFFER, buffer);
    }

    void unbind() const {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
};

class FrameBuffer {
    struct Attachment {
        enum Type {
            TEXTURE,
            RENDER_BUFFER,
        };

        union {
            Texture * texture;
            RenderBuffer * buffer;
        };
        Type type;
        GLenum attachment;

        Attachment(Texture * texture, GLenum attachment)
            : texture(texture), type(TEXTURE), attachment(attachment) {}

        Attachment(RenderBuffer * buffer, GLenum attachment)
            : buffer(buffer), type(RENDER_BUFFER), attachment(attachment) {}

        void resize(int width, int height) {
            if (type == TEXTURE)
                texture->resize({width, height});
            else
                buffer->resize(width, height);
        }
    };

    GLuint buffer;
    std::vector<Attachment> attachments;

    FrameBuffer(GLuint buffer) : buffer(buffer) {}

public:
    FrameBuffer() {
        glGenFramebuffers(1, &buffer);
        bind();
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

    void attach(Texture * texture, GLenum attachment = GL_COLOR_ATTACHMENT0) {
        attachments.emplace_back(texture, attachment);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               attachment,
                               texture->getTarget(),
                               texture->getTextureId(),
                               0);
    }

    void attach(RenderBuffer * buffer,
                GLenum attachment = GL_DEPTH_STENCIL_ATTACHMENT) {
        attachments.emplace_back(buffer, attachment);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  attachment,
                                  GL_RENDERBUFFER,
                                  buffer->getBufferId());
    }

    void resize(int width, int height) {
        for (auto & att : attachments) {
            att.resize(width, height);
        }
    }

    void bind(GLenum target = GL_FRAMEBUFFER) const {
        glBindFramebuffer(target, buffer);
    }

    void unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    static const FrameBuffer & getDefault() {
        static FrameBuffer buffer(0);
        return buffer;
    }
};
