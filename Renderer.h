//
// Created by Quinta on 7/12/2024.
//

#ifndef GRAVITY_RENDERER_H
#define GRAVITY_RENDERER_H
#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Simulator.h"

class Renderer {
public:
    Renderer(int width, int height);
    ~Renderer();

    void render(const Simulator& simulator);
    bool shouldClose();
    void swapBuffers();
    void processInput();
    void cursorPosCallback(double xpos, double ypos);

private:
    GLFWwindow* window;
    void drawSphere(const glm::vec3& position, float radius);
    void createSphereMesh(float radius, int sectors, int stacks);
    void drawDebugTriangle();  // Debug function to draw a triangle

    GLuint sphereVAO, sphereVBO, sphereEBO;
    int sphereVertexCount, sphereIndexCount;

    void drawGrid(const Simulator& simulator);
    float calculateGravityFieldStrength(const glm::vec3& point, const std::vector<CelestialBody>& bodies);
    void drawGravityField(const Simulator& simulator);
    void drawTrajectories(const std::vector<CelestialBody>& bodies);

    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    float cameraSpeed;
    float mouseSensitivity;
    float yaw;
    float pitch;
    bool firstMouse;
    double lastX, lastY;

    void updateCameraVectors();
};
#endif //GRAVITY_RENDERER_H
