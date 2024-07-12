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

CelestialBody::CelestialBody(double mass, const glm::dvec3& position, const glm::dvec3& velocity, double radius)
        : mass(mass), position(position), velocity(velocity), acceleration(0.0f) {}

void CelestialBody::update(double dt) {
    // Runge-Kutta 4th order method
    glm::dvec3 k1v = acceleration * dt;
    glm::dvec3 k1r = velocity * dt;

    glm::dvec3 k2v = acceleration * dt;
    glm::dvec3 k2r = (velocity + k1v * 0.5) * dt;

    glm::dvec3 k3v = acceleration * dt;
    glm::dvec3 k3r = (velocity + k2v * 0.5) * dt;

    glm::dvec3 k4v = acceleration * dt;
    glm::dvec3 k4r = (velocity + k3v) * dt;

    velocity += (k1v + 2.0 * k2v + 2.0 * k3v + k4v) / 6.0;
    position += (k1r + 2.0 * k2r + 2.0 * k3r + k4r) / 6.0;

    acceleration = glm::dvec3(0.0);

}

void CelestialBody::applyForce(const glm::dvec3& force) {
    acceleration += force / mass;
}

void CelestialBody::addToTrajectory(const glm::dvec3& position) {
    trajectory.push_back(position);
    if (trajectory.size() > MAX_TRAJECTORY_POINTS) {
        trajectory.erase(trajectory.begin());
    }
}