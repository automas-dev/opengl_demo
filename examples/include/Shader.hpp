#pragma once

#include <GL/glew.h>
// gl.h after glew.h, clang-format don't sort
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <stdexcept>
#include <string>

class Shader {
public:
    class Uniform {
        GLuint location;

    public:
        Uniform(GLuint location) : location(location) {}

        GLuint getLocation() const {
            return location;
        }

        void setValue(bool value) const {
            glUniform1i(location, static_cast<int>(value));
        }

        void setValue(int value) const {
            glUniform1i(location, value);
        }

        void setValue(unsigned int value) const {
            glUniform1ui(location, value);
        }

        void setValue(float value) const {
            glUniform1f(location, value);
        }

        void setValue(double value) const {
            glUniform1d(location, value);
        }

        void setVec2(const glm::vec2 & value) const {
            glUniform2fv(location, 1, &value.x);
        }

        void setVec3(const glm::vec3 & value) const {
            glUniform3fv(location, 1, &value.x);
        }

        void setVec4(const glm::vec4 & value) const {
            glUniform4fv(location, 1, &value.x);
        }

        void setMat2(const glm::mat2 & value) const {
            glUniformMatrix2fv(location, 1, GL_FALSE, &value[0][0]);
        }

        void setMat3(const glm::mat3 & value) const {
            glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
        }

        void setMat4(const glm::mat4 & value) const {
            glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
        }
    };

private:
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
        if (program)
            glDeleteProgram(program);
    }

    Shader(Shader && other) {
        program = other.program;
        other.program = 0;
    }

    Shader(const Shader & other) = delete;
    Shader & operator=(const Shader & other) = delete;
    Shader & operator=(Shader && other) = delete;

    GLuint getProgram() const {
        return program;
    }

    void bind() const {
        glUseProgram(program);
    }

    void unbind() const {
        glUseProgram(0);
    }

    Uniform uniform(const char * name) const {
        GLuint location = glGetUniformLocation(program, name);
        return Uniform(location);
    }

public:
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
