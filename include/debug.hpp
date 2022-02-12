#pragma once

#include <GL/gl.h>

#include <iostream>

static void GLAPIENTRY MessageCallback(GLenum source,
                                       GLenum type,
                                       GLuint id,
                                       GLenum severity,
                                       GLsizei length,
                                       const GLchar * message,
                                       const void * userParam) {

    std::cerr << "GL Error: "
              << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
              << " type = " << type << " severity = " << severity
              << " message = " << message << std::endl;
}

void initDebug() {
    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
}
