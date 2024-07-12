//
// Created by Quinta on 7/12/2024.
//

#ifndef GRAVITY_CELESTIALBODY_H
#define GRAVITY_CELESTIALBODY_H
#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

class CelestialBody {
public:
    CelestialBody(double mass, const glm::dvec3& position, const glm::dvec3& velocity, double radius);

    void update(double dt);
    void applyForce(const glm::dvec3& force);

    [[nodiscard]] double getMass() const { return mass; }
    [[nodiscard]] glm::dvec3 getPosition() const { return position; }
    [[nodiscard]] glm::dvec3 getVelocity() const { return velocity; }
    void addToTrajectory(const glm::dvec3& position);
    const std::vector<glm::dvec3>& getTrajectory() const { return trajectory; }
    double getRadius() const { return radius; }

private:
    double mass;
    glm::dvec3 position;
    glm::dvec3 velocity;
    glm::dvec3 acceleration;
    std::vector<glm::dvec3> trajectory;
    static const size_t MAX_TRAJECTORY_POINTS = 1000;
    double radius;
};
#endif //GRAVITY_CELESTIALBODY_H
