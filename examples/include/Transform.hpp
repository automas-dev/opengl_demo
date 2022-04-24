#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

class Transform {
    glm::vec3 m_position;
    glm::quat m_rotation;
    glm::vec3 m_scale;

    mutable glm::mat4 matrix;
    mutable bool changed;

public:
    Transform()
        : Transform(glm::vec3(0), glm::quat(glm::vec3(0)), glm::vec3(1)) {}

    Transform(const glm::vec3 & position,
              const glm::quat & rotation,
              const glm::vec3 & scale)
        : m_position(position),
          m_rotation(rotation),
          m_scale(scale),
          changed(true) {}

    Transform(const glm::mat4 & matrix) : matrix(matrix), changed(false) {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(matrix, m_scale, m_rotation, m_position, skew, perspective);
        m_rotation = glm::conjugate(m_rotation);
    }

    Transform(const Transform & other) = default;

    Transform(Transform && other) = default;

    Transform & operator=(const Transform & other) = default;

    Transform & operator=(Transform && other) = default;

    ~Transform() {}

    void move(const glm::vec3 & delta) {
        m_position += delta;
        changed = true;
    }

    void rotateEuler(const glm::vec3 & delta) {
        rotate(glm::quat(delta));
        changed = true;
    }

    void rotate(const glm::quat & delta) {
        m_rotation = delta * m_rotation;
        changed = true;
    }

    void scale(const glm::vec3 & scale) {
        m_scale *= scale;
        changed = true;
    }

    const glm::vec3 & getPosition() const {
        return m_position;
    }

    void setPosition(const glm::vec3 & position) {
        m_position = position;
        changed = true;
    }

    glm::vec3 getRotationEuler() const {
        return glm::eulerAngles(m_rotation);
    }

    const glm::quat & getRotation() const {
        return m_rotation;
    }

    void setRotation(const glm::quat & rotation) {
        m_rotation = rotation;
        changed = true;
    }

    const glm::vec3 & getScale() const {
        return m_scale;
    }

    void setScale(const glm::vec3 & scale) {
        m_scale = scale;
        changed = true;
    }

    glm::mat4 toMatrix() const {
        if (changed) {
            auto translate = glm::translate(glm::mat4(1), m_position);
            auto rotate = glm::toMat4(m_rotation);
            auto scale = glm::scale(glm::mat4(1), m_scale);
            matrix = translate * rotate * scale;
            changed = false;
        }

        return matrix;
    }
};
