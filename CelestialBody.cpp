//
// Created by Quinta on 7/12/2024.
//
#include "CelestialBody.h"
#include <iostream>
#include <sstream>

// Helper function to convert glm::vec3 to string
std::string vec3_to_string(const glm::vec3& v) {
    std::stringstream ss;
    ss << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return ss.str();
}

CelestialBody::CelestialBody(float mass, const glm::vec3& position, const glm::vec3& velocity)
        : mass(mass), position(position), velocity(velocity), acceleration(0.0f) {}

void CelestialBody::update(float dt) {
    if (glm::any(glm::isnan(velocity)) || glm::any(glm::isinf(velocity))) {
        std::cout << "Warning: Invalid velocity detected: " << vec3_to_string(velocity) << std::endl;
        velocity = glm::vec3(0.0f);
    }

    velocity += acceleration * dt;
    position += velocity * dt;

    if (glm::any(glm::isnan(position)) || glm::any(glm::isinf(position))) {
        std::cout << "Warning: Invalid position detected: " << vec3_to_string(position) << std::endl;
        position = glm::vec3(0.0f);
    }

    acceleration = glm::vec3(0.0f);
}

void CelestialBody::applyForce(const glm::vec3& force) {
    acceleration += force / mass;
}