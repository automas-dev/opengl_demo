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
    int width, height;

public:
    RenderBuffer(int width, int height, GLenum internal)
        : internal(internal), width(width), height(height) {
        glGenRenderbuffers(1, &buffer);
        resize(width, height);
    }

    RenderBuffer(RenderBuffer && other)
        : buffer(other.buffer),
          internal(other.internal),
          width(other.width),
          height(other.height) {
        other.buffer = 0;
    }

    RenderBuffer & operator=(RenderBuffer && other) {
        buffer = other.buffer;
        other.buffer = 0;
        internal = other.internal;
        width = other.width;
        height = other.height;
        return *this;
    }

    RenderBuffer(const RenderBuffer &) = delete;
    RenderBuffer & operator=(const RenderBuffer &) = delete;

    ~RenderBuffer() {
        if (buffer)
            glDeleteRenderbuffers(1, &buffer);
    }

    GLuint getBufferId() const {
        return buffer;
    }

    int getWidth() const {
        return width;
    }

    int getHeight() const {
        return height;
    }

    void resize(int width, int height) {
        this->width = width;
        this->height = height;
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
    int width, height;

    FrameBuffer(GLuint buffer) : buffer(buffer) {}

public:
    FrameBuffer(int width, int height) : width(width), height(height) {
        glGenFramebuffers(1, &buffer);
        bind();
    }

    FrameBuffer(FrameBuffer && other)
        : buffer(other.buffer),
          attachments(std::move(other.attachments)),
          width(other.width),
          height(other.height) {
        other.buffer = 0;
    }

    FrameBuffer & operator=(FrameBuffer && other) {
        buffer = other.buffer;
        other.buffer = 0;
        attachments = std::move(other.attachments);
        width = other.width;
        height = other.height;
        return *this;
    }

    FrameBuffer(const FrameBuffer &) = delete;
    FrameBuffer & operator=(const FrameBuffer &) = delete;

    ~FrameBuffer() {
        if (buffer)
            glDeleteFramebuffers(1, &buffer);
    }

    GLuint getBufferId() const {
        return buffer;
    }

    void attach(Texture * texture, GLenum attachment = GL_COLOR_ATTACHMENT0) {
        if (texture->getSize().x != width || texture->getSize().y != height)
            throw std::runtime_error("Attachment size does not match");

        attachments.emplace_back(texture, attachment);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               attachment,
                               texture->getTarget(),
                               texture->getTextureId(),
                               0);
    }

    void attach(RenderBuffer * buffer,
                GLenum attachment = GL_DEPTH_STENCIL_ATTACHMENT) {
        if (buffer->getWidth() != width || buffer->getHeight() != height)
            throw std::runtime_error("Attachment size does not match");

        attachments.emplace_back(buffer, attachment);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  attachment,
                                  GL_RENDERBUFFER,
                                  buffer->getBufferId());
    }

    void resize(int width, int height) {
        this->width = width;
        this->height = height;
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

    void blit(const FrameBuffer & source,
              GLbitfield mask = GL_COLOR_BUFFER_BIT,
              GLenum filter = GL_NEAREST) const {
        source.bind(GL_READ_FRAMEBUFFER);
        bind(GL_DRAW_FRAMEBUFFER);
        glBlitFramebuffer(0, 0, source.width, source.height, //
                          0, 0, width, height, //
                          mask, filter);
    }

    static FrameBuffer & getDefault() {
        static FrameBuffer buffer(0);
        return buffer;
    }
};
