//
// Created by Quinta on 7/12/2024.
//
#include "Simulator.h"
#include "Renderer.h"
#include <chrono>
#include <thread>

glm::dvec3 calculateOrbitalVelocity(double centralMass, double distance) {
    const double G = 6.67430e-11;
    double speed = std::sqrt(G * centralMass / distance);
    return glm::dvec3(0, speed, 0);  // Assuming orbit in the XZ plane
}

int main() {
    Simulator simulator;
    Renderer renderer(1600, 1200);

    double sunMass = 1.989e30;

    // Sun (at the center)
    simulator.addBody(CelestialBody(sunMass, glm::dvec3(0, 0, 0), glm::dvec3(0, 0, 0), 6.96e8));

    // Mercury
    double mercuryDist = 57.9e9;
    simulator.addBody(CelestialBody(3.285e23, glm::dvec3(mercuryDist, 0, 0), calculateOrbitalVelocity(sunMass, mercuryDist), 2.44e6));

    // Venus
    double venusDist = 108.2e9;
    simulator.addBody(CelestialBody(4.867e24, glm::dvec3(venusDist, 0, 0), calculateOrbitalVelocity(sunMass, venusDist), 6.05e6));

    // Earth
    double earthDist = 149.6e9;
    simulator.addBody(CelestialBody(5.972e24, glm::dvec3(earthDist, 0, 0), calculateOrbitalVelocity(sunMass, earthDist), 6.37e6));

    // Mars
    double marsDist = 227.9e9;
    simulator.addBody(CelestialBody(6.39e23, glm::dvec3(marsDist, 0, 0), calculateOrbitalVelocity(sunMass, marsDist), 3.39e6));

    // Jupiter
    double jupiterDist = 778.5e9;
    simulator.addBody(CelestialBody(1.898e27, glm::dvec3(jupiterDist, 0, 0), calculateOrbitalVelocity(sunMass, jupiterDist), 69.91e6));

    // Saturn
    double saturnDist = 1.429e12;
    simulator.addBody(CelestialBody(5.683e26, glm::dvec3(saturnDist, 0, 0), calculateOrbitalVelocity(sunMass, saturnDist), 58.23e6));

    // Uranus
    double uranusDist = 2.871e12;
    simulator.addBody(CelestialBody(8.681e25, glm::dvec3(uranusDist, 0, 0), calculateOrbitalVelocity(sunMass, uranusDist ), 25.36e6));

    // Neptune
    double neptuneDist = 4.495e12;
    simulator.addBody(CelestialBody(1.024e26, glm::dvec3(neptuneDist, 0, 0), calculateOrbitalVelocity(sunMass, neptuneDist), 24.62e6));

    // Pluto
    double plutoDist = 5.906e12;
    simulator.addBody(CelestialBody(1.309e22, glm::dvec3(plutoDist, 0, 0), calculateOrbitalVelocity(sunMass, plutoDist), 1.18e6));



    const float dt = 3600.0f; // Time step of 1 hour

    while (!renderer.shouldClose()) {
        renderer.processInput();
        simulator.update(dt);
        renderer.render(simulator);
        renderer.swapBuffers();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}