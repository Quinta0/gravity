//
// Created by Quinta on 7/12/2024.
//
#include "Simulator.h"
#include <glm/glm.hpp>
#include <iostream>

Simulator::Simulator() {}

void Simulator::addBody(const CelestialBody& body) {
    bodies.push_back(body);
}

void Simulator::update(float dt) {
    // Calculate and apply gravitational forces
    for (size_t i = 0; i < bodies.size(); ++i) {
        glm::vec3 totalForce(0.0f);
        for (size_t j = 0; j < bodies.size(); ++j) {
            if (i != j) {
                glm::vec3 force = calculateGravitationalForce(bodies[i], bodies[j]);
                totalForce += force;
            }
        }
        bodies[i].applyForce(totalForce);
    }

    // Update positions and velocities
    for (auto& body : bodies) {
        body.update(dt);
    }
}

glm::vec3 Simulator::calculateGravitationalForce(const CelestialBody& body1, const CelestialBody& body2) {
    glm::vec3 direction = body2.getPosition() - body1.getPosition();
    float distance = glm::length(direction);

    // Avoid division by zero and unrealistic forces at very small distances
    if (distance < 1e9) {
        std::cout << "Warning: Bodies too close, using minimum distance" << std::endl;
        distance = 1e9;
    }

    // Use the actual G value
    const float G = 6.67430e-11f;
    float forceMagnitude = G * (body1.getMass() * body2.getMass()) / (distance * distance);

    if (std::isnan(forceMagnitude) || std::isinf(forceMagnitude)) {
        std::cout << "Warning: Invalid force magnitude calculated. Distance: " << distance
                  << ", Masses: " << body1.getMass() << ", " << body2.getMass() << std::endl;
        return glm::vec3(0.0f);
    }

    return glm::normalize(direction) * forceMagnitude;
}