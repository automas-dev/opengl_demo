#pragma once

#include <GL/glew.h>
// gl.h after glew.h, clang-format don't sort
#include <GL/gl.h>

#include <stdexcept>
#include <string>

class Shader {
    GLuint program;

public:
    Shader(const char * vertexSource, const char * fragmentSource) {
        GLuint vShader = compileShader(GL_VERTEX_SHADER, vertexSource);
        GLuint fShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

        program = glCreateProgram();

        glAttachShader(program, vShader);
        glAttachShader(program, fShader);

        glLinkProgram(program);

        glDetachShader(program, vShader);
        glDetachShader(program, fShader);
        glDeleteShader(vShader);
        glDeleteShader(fShader);

        if (!linkSuccess(program)) {
            throw LinkException(program);
        }
    }

    ~Shader() {
        glDeleteProgram(program);
    }

    Shader(Shader && other) = default;

    Shader & operator=(Shader && other) = default;

    GLuint getProgram() const {
        return program;
    }

    void bind() const {
        glUseProgram(program);
    }

    void unbind() const {
        glUseProgram(0);
    }

    GLuint uniform(const char * name) const {
        return glGetUniformLocation(program, name);
    }

    class CompileException : public std::runtime_error {
        std::string compileError(GLuint shader) {
            GLint logSize = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

            std::vector<GLchar> errorLog(logSize);
            glGetShaderInfoLog(shader, logSize, &logSize, &errorLog[0]);

            return std::string(errorLog.begin(), errorLog.end());
        }

    public:
        CompileException(GLuint shader)
            : std::runtime_error(compileError(shader)) {}
    };

    class LinkException : public std::runtime_error {
        std::string linkError(GLuint program) {
            GLint logSize = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);

            std::vector<GLchar> errorLog(logSize);
            glGetProgramInfoLog(program, logSize, &logSize, &errorLog[0]);

            return std::string(errorLog.begin(), errorLog.end());
        }

    public:
        LinkException(GLuint program)
            : std::runtime_error(linkError(program)) {}
    };

private:
    bool compileSuccess(GLuint shader) {
        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        return success != GL_FALSE;
    }

    bool linkSuccess(GLuint program) {
        GLint success = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        return success != GL_FALSE;
    }

    GLuint compileShader(GLuint shaderType, const char * shaderSource) {
        GLuint shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &shaderSource, NULL);
        glCompileShader(shader);
        if (!compileSuccess(shader)) {
            throw CompileException(shader);
        }
        return shader;
    }
};
