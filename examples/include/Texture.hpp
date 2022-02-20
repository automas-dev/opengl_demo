#pragma once

#include <GL/glew.h>
// gl.h after glew.h, clang-format don't sort
#include <GL/gl.h>
// REMEMBER TO DEVINE STB_IMAGE_IMPLEMENTATION in main.cpp
#include <stb_image.h>

#include <glm/glm.hpp>
#include <stdexcept>

class Texture {
public:
    enum Format {
        Gray = GL_RED,
        RGB = GL_RGB,
        RGBA = GL_RGBA,
    };

    /// Mag filter only accepts Nearest or Linear.
    enum Filter {
        // Min and Mag filter
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR,

        // Min filter only
        NearestMmNearest = GL_NEAREST_MIPMAP_NEAREST,
        LinearMmNearest = GL_LINEAR_MIPMAP_NEAREST,
        NearestMmLinear = GL_NEAREST_MIPMAP_LINEAR,
        LinearMmLinear = GL_LINEAR_MIPMAP_LINEAR,
    };

    enum Wrap {
        Clamp = GL_CLAMP_TO_EDGE,
        Border = GL_CLAMP_TO_BORDER,
        MirrorRepeat = GL_MIRRORED_REPEAT,
        Repeat = GL_REPEAT,
        MirrorClamp = GL_MIRROR_CLAMP_TO_EDGE
    };

private:
    GLuint textureId;
    glm::uvec2 size;
    Format internal;
    Format format;
    GLenum type;
    GLsizei samples;
    GLenum target;

    Filter magFilter, minFilter;
    Wrap wrap;
    bool mipmaps;

public:
    /**
     * Create a texture from an image.
     *
     * Throw TextureLoadException if nrComponents is unsupported. Only 1, 3
     * and 4 are supported.
     *
     * @param data the pixel data
     * @param size the image dimensions in pixesl
     * @param nrComponents the number of components for each pixel
     * @param magFilter the magnification filter (default GL_NEAREST)
     * @param minFilter the minification filter (default GL_NEAREST)
     * @param wrap the wrap mode when drawing
     * @param mipmaps should mipmaps be generated
     *
     * @throws TextureLoadException for unsupported nrComponents
     */
    Texture(const unsigned char * data,
            const glm::uvec2 & size,
            size_t nrComponents,
            Filter magFilter = Linear,
            Filter minFilter = LinearMmLinear,
            Wrap wrap = Repeat,
            bool mipmaps = true)
        : textureId(0),
          size(size),
          internal(RGBA),
          format(RGBA),
          type(GL_UNSIGNED_BYTE),
          samples(0),
          target(GL_TEXTURE_2D),
          minFilter(minFilter),
          magFilter(magFilter),
          wrap(wrap),
          mipmaps(mipmaps) {

        glGenTextures(1, &textureId);
        loadFrom(data, size, nrComponents);
    }

    /**
     * Create an empty texture of the specified size.
     *
     * Note: if samples is > 0, the filter, wrap and mipmaps arguments are
     * ignored.
     *
     * @param size the image size in pixels
     * @param internal the internal format like GL_RGBA16F, GL_RGBA,
     *                 GL_DEPTH_COMPONENT, etc.
     * @param format the format of pixel data
     * @param type the data type of pixel data
     * @param samples the number of samples to use, 0 to disable
     *                multisampling
     * @param magFilter the magnification filter (default GL_NEAREST)
     * @param minFilter the minification filter (default GL_NEAREST)
     * @param wrap the wrap mode when drawing
     * @param mipmaps should mipmaps be generated
     */
    Texture(const glm::uvec2 & size,
            Format internal = RGBA,
            Format format = RGBA,
            GLenum type = GL_FLOAT,
            GLsizei samples = 0,
            Filter magFilter = Linear,
            Filter minFilter = LinearMmLinear,
            Wrap wrap = Repeat,
            bool mipmaps = true)
        : textureId(0),
          size(size),
          internal(internal),
          format(format),
          type(type),
          samples(samples),
          target(samples > 0 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D),
          minFilter(minFilter),
          magFilter(magFilter),
          wrap(wrap),
          mipmaps(mipmaps) {

        glGenTextures(1, &textureId);
        resize(size);
    }

    ~Texture() {
        if (textureId)
            glDeleteTextures(1, &textureId);
    }

    Texture(Texture && other) {
        textureId = other.textureId;
        other.textureId = 0;
        size = other.size;
        internal = other.internal;
        format = other.format;
        type = other.type;
        samples = other.samples;
        target = other.target;
        magFilter = other.magFilter;
        minFilter = other.minFilter;
        wrap = other.wrap;
        mipmaps = other.mipmaps;
    }

    Texture(const Texture & other) = delete;
    Texture & operator=(const Texture & other) = delete;
    Texture & operator=(Texture && other) = delete;

    GLuint getTextureId() const {
        return textureId;
    }

    const glm::uvec2 & getSize() const {
        return size;
    }

    void bind() const {
        glBindTexture(target, textureId);
    }

    void unbind() const {
        glBindTexture(target, 0);
    }

    /**
     * Load the texture from an image, setting the size to match
     * image.getSize().
     *
     * Throw TextureLoadException if nrComponents is unsupported. Only 1, 3
     * and 4 are supported.
     *
     * @param data the pixel data
     * @param size the image dimensions in pixesl
     * @param nrComponents the number of components for each pixel
     *
     * @throws TextureLoadException for unsupported nrComponents
     */
    void loadFrom(const unsigned char * data,
                  const glm::uvec2 & size,
                  size_t nrComponents) {
        bind();

        this->size = size;
        if (nrComponents == 1)
            internal = Gray;
        else if (nrComponents == 3)
            internal = RGB;
        else if (nrComponents == 4)
            internal = RGBA;
        else
            throw TextureLoadException("Unsupported number of components");
        format = internal;
        type = GL_UNSIGNED_BYTE;
        samples = 0;
        target = GL_TEXTURE_2D;

        glTexImage2D(target, 0, internal, size.x, size.y, 0, format, type, data);

        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);

        glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);

        if (mipmaps)
            glGenerateMipmap(target);
        unbind();
    }

    void resize(const glm::uvec2 & size) {
        this->size = size;
        if (size.x > 0 && size.y > 0) {
            bind();
            if (samples > 0) {
                glTexImage2DMultisample(target, samples, internal, size.x,
                                        size.y, GL_TRUE);
            }
            else {
                glTexImage2D(target, 0, internal, size.x, size.y, 0, format,
                             type, NULL);

                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);

                glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
                glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
            }
            unbind();
        }
    }

    /**
     * Load the texture from a file, setting the size from the image.
     *
     * Throw TextureLoadException if the image has an unsupported number of
     * components. Only 1, 3 and 4 are supported.
     *
     * @param path the path to the image file
     *
     * @throws TextureLoadException if the image han an unsupported number
     * of components
     */
    static Texture fromPath(const std::string & path) {
        int x, y, n;
        auto * data = stbi_load(path.c_str(), &x, &y, &n, 0);
        if (!data)
            throw TextureLoadException("Failed to load image from file");
        return Texture(data, glm::uvec2(x, y), n);
    }

    class TextureLoadException : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };
};
