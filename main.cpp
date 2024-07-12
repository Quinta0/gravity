//
// Created by Quinta on 7/12/2024.
//
#include "Simulator.h"
#include "Renderer.h"
#include <chrono>
#include <thread>

int main() {
    Simulator simulator;
    Renderer renderer(1600, 1200);  // Increased window size for better visibility

// Sun (at the center)
    simulator.addBody(CelestialBody(1.989e30f, glm::dvec3(0, 0, 0), glm::dvec3(0, 0, 0)));

// Mercury
    simulator.addBody(CelestialBody(3.285e23f, glm::dvec3(57.9e9f, 0, 0), glm::dvec3(0, 47.36e3f, 0)));

// Venus
    simulator.addBody(CelestialBody(4.867e24f, glm::dvec3(108.2e9f, 0, 0), glm::dvec3(0, 35.02e3f, 0)));

// Earth
    simulator.addBody(CelestialBody(5.972e24f, glm::dvec3(149.6e9f, 0, 0), glm::dvec3(0, 29.78e3f, 0)));

// Mars
    simulator.addBody(CelestialBody(6.39e23f, glm::dvec3(227.9e9f, 0, 0), glm::dvec3(0, 24.07e3f, 0)));

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