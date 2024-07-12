//
// Created by 0piet on 7/12/2024.
//

#ifndef GRAVITY_CELESTIALBODY_H
#define GRAVITY_CELESTIALBODY_H
#pragma once
#include <glm/glm.hpp>
#include <string>

class CelestialBody {
public:
    CelestialBody(float mass, const glm::vec3& position, const glm::vec3& velocity);

    void update(float dt);
    void applyForce(const glm::vec3& force);

    float getMass() const { return mass; }
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getVelocity() const { return velocity; }

private:
    float mass;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
};
#endif //GRAVITY_CELESTIALBODY_H
