//
// Created by Quinta on 7/12/2024.
//

#ifndef GRAVITY_SIMULATOR_H
#define GRAVITY_SIMULATOR_H
#pragma once
#include <vector>
#include "CelestialBody.h"

class Simulator {
public:
    Simulator();

    void addBody(const CelestialBody& body);
    void update(double dt);
    const std::vector<CelestialBody>& getBodies() const { return bodies; }
    glm::dvec3 calculateGravitationalForce(const CelestialBody& body1, const CelestialBody& body2);


private:
    std::vector<CelestialBody> bodies;
    const float G = 6.67430e-11f; // Gravitational constant
    void checkCollisions();
    void handleCollision(CelestialBody& body1, CelestialBody& body2);
};
#endif //GRAVITY_SIMULATOR_H
