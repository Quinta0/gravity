//
// Created by Quinta on 7/12/2024.
//
#include "Simulator.h"
#include <glm/glm.hpp>
#include <iostream>
#include <algorithm>

Simulator::Simulator() {}

void Simulator::addBody(const CelestialBody& body) {
    bodies.push_back(body);
}

void Simulator::update(double dt) {
    // Sort bodies by mass (descending order)
    std::sort(bodies.begin(), bodies.end(), [](const CelestialBody& a, const CelestialBody& b) {
        return a.getMass() > b.getMass();
    });

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
    for (size_t i = 1; i < bodies.size(); ++i) { // Start from 1 to skip the Sun
        bodies[i].update(dt);
        bodies[i].addToTrajectory(bodies[i].getPosition());
    }
}

glm::dvec3 Simulator::calculateGravitationalForce(const CelestialBody& body1, const CelestialBody& body2) {
    glm::dvec3 direction = body2.getPosition() - body1.getPosition();
    double distance = glm::length(direction);

    // Avoid division by zero and unrealistic forces at very small distances
    if (distance < 1e9) {
        std::cout << "Warning: Bodies too close, using minimum distance" << std::endl;
        distance = 1e9;
    }

    // Use the actual G value
    const double G = 6.67430e-11;
    double forceMagnitude = G * (body1.getMass() * body2.getMass()) / (distance * distance);

    if (std::isnan(forceMagnitude) || std::isinf(forceMagnitude)) {
        std::cout << "Warning: Invalid force magnitude calculated. Distance: " << distance
                  << ", Masses: " << body1.getMass() << ", " << body2.getMass() << std::endl;
        return glm::dvec3(0.0);
    }

    return glm::normalize(direction) * forceMagnitude;
}