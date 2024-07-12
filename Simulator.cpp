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

    // Check for collisions
    checkCollisions();
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
        return glm::dvec3(0.0);
    }

    return glm::normalize(direction) * forceMagnitude;
}

void Simulator::handleCollision(CelestialBody& body1, CelestialBody& body2) {
    double totalMass = body1.getMass() + body2.getMass();

    // Calculate center of mass position
    glm::dvec3 newPosition = (body1.getPosition() * body1.getMass() + body2.getPosition() * body2.getMass()) / totalMass;

    // Calculate new velocity (momentum conservation)
    glm::dvec3 newVelocity = (body1.getVelocity() * body1.getMass() + body2.getVelocity() * body2.getMass()) / totalMass;

    // Calculate new radius (assuming constant density)
    double newRadius = std::pow(std::pow(body1.getRadius(), 3) + std::pow(body2.getRadius(), 3), 1.0/3.0);

    // Create new body
    CelestialBody newBody(totalMass, newPosition, newVelocity, newRadius);

    // Replace body1 with the new body
    body1 = newBody;

    // Remove body2
    auto it = std::find_if(bodies.begin(), bodies.end(), [&body2](const CelestialBody& b) {
        return &b == &body2;
    });
    if (it != bodies.end()) {
        bodies.erase(it);
    }
}

void Simulator::checkCollisions() {
    for (size_t i = 0; i < bodies.size(); ++i) {
        for (size_t j = i + 1; j < bodies.size(); ++j) {
            CelestialBody& body1 = bodies[i];
            CelestialBody& body2 = bodies[j];
            glm::dvec3 distanceVec = body1.getPosition() - body2.getPosition();
            double distance = glm::length(distanceVec);
            if (distance < (body1.getRadius() + body2.getRadius())) {
                handleCollision(body1, body2);
            }
        }
    }
}