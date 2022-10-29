#pragma once

#include "glm/glm.hpp"

struct RigidBodyComponent {
    glm::vec2 velocity{};

    explicit RigidBodyComponent(glm::vec2 velocity = glm::vec2{0}) {
        this->velocity = velocity;
    }
};
