#include <iostream>
using namespace std;

#include <GL/glew.h>

#include <SFML/Graphics.hpp>
#include <Shader.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <Buffer.hpp>
#include <Texture.hpp>
#include <debug.hpp>

static const char * vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
out vec2 FragTex;
void main() {
    gl_Position = vec4(aPos, 1.0);
    FragTex = aTex;
})";

static const char * fragmentShaderSource = R"(
#version 330 core
in vec2 FragTex;
out vec4 FragColor;
uniform sampler2D gTexture;
void main() {
    FragColor = texture(gTexture, FragTex);
})";

static const char * screenVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
out vec2 FragPos;
out vec2 FragTex;
void main() {
    gl_Position = vec4(aPos, 1.0);
    FragPos = aPos.xy;
    FragTex = aTex;
})";

static const char * screenFragmentShaderSource = R"(
#version 330 core
in vec2 FragPos;
in vec2 FragTex;
out vec4 FragColor;
uniform sampler2D gTexture;
uniform float t;
void main() {
    vec2 d = vec2(sin(t + FragPos.x * 3) * 0.1, sin(t + FragPos.y * 3) * 0.1);
    vec2 texCoord = FragTex + vec2(d.x, 0.0);
    vec4 c = texture(gTexture, texCoord);
    float v = (c.r + c.g + c.b) / 3.0;
    FragColor = vec4(vec3(v), c.a);
})";

int main() {
    const sf::ContextSettings settings(24, 1, 8, 3, 0);
    sf::RenderWindow window(sf::VideoMode(800, 600),
                            "Post Processing",
                            sf::Style::Default,
                            settings);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);
    window.setActive();
    window.setKeyRepeatEnabled(false);

    // glewExperimental = true;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        cerr << "glewInit failed: " << glewGetErrorString(err);
        return 1;
    }

    initDebug();

    Shader shader(vertexShaderSource, fragmentShaderSource);
    Shader screenShader(screenVertexShaderSource, screenFragmentShaderSource);
    GLuint sst = screenShader.uniform("t");
    Texture texture = Texture::fromPath("../../../examples/res/uv.png");

    const float vertices[] = {
        -0.5f, -0.5f, 0.0f, // Bottom Left
        0.5f,  -0.5f, 0.0f, // Bottom Right
        0.0f,  0.5f,  0.0f // Top Center
    };

    const float texCoords[] = {
        -0.5f, -0.5f, // Bottom Left
        0.5f,  -0.5f, // Bottom Right
        0.0f,  0.5f, // Top Center
    };

    const unsigned int indices[] = {
        0, 1, 2, // First Triangle
    };

    Attribute a0 {0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0};
    Attribute a1 {1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0};

    BufferArray array(std::vector<Attribute> {a0, a1});
    array.bind();
    array.bufferData(0, sizeof(vertices), vertices);
    array.bufferData(1, sizeof(texCoords), texCoords);
    array.bufferElements(sizeof(indices), indices);
    array.unbind();

    Quad quad;

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    Texture fboTexture(glm::vec2(window.getSize().x, window.getSize().y),
                       Texture::RGB,
                       Texture::RGB,
                       GL_FLOAT,
                       0,
                       Texture::Linear,
                       Texture::Linear,
                       Texture::Clamp,
                       false);

    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           fboTexture.getTextureId(),
                           0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER,
                          GL_DEPTH24_STENCIL8,
                          window.getSize().x,
                          window.getSize().y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER,
                              rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "FBO is not complete!" << std::endl;
        return 1;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Escape)
                        window.close();
                    break;
                case sf::Event::Resized: {
                    sf::FloatRect visibleArea(0, 0, event.size.width,
                                              event.size.height);
                    window.setView(sf::View(visibleArea));
                    glViewport(0, 0, event.size.width, event.size.height);
                } break;
                case sf::Event::Closed:
                    window.close();
                    break;
                default:
                    break;
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.bind();
        texture.bind();
        array.drawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.bind();
        glUniform1f(sst, clock.getElapsedTime().asSeconds());
        fboTexture.bind();
        quad.draw();

        window.display();
    }

    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);

    window.close();

    return 0;
}
