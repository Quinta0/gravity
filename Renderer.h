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

private:
    GLFWwindow* window;
    void drawSphere(const glm::vec3& position, float radius);
    void createSphereMesh(float radius, int sectors, int stacks);
    void drawDebugTriangle();  // Add this line

    GLuint sphereVAO, sphereVBO, sphereEBO;
    int sphereVertexCount, sphereIndexCount;

    void drawGrid();
};
#endif //GRAVITY_RENDERER_H
