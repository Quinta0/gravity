//
// Created by Quinta on 7/12/2024.
//
#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <iostream>

Renderer::Renderer(int width, int height) {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    window = glfwCreateWindow(width, height, "3D Gravity Simulator", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW");
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    createSphereMesh(1.0f, 20, 20);
}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &sphereEBO);
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Renderer::render(const Simulator& simulator) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1024.0 / 768.0, 1e8, 1e12);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(3e11, 2e11, 3e11, 0, 0, 0, 0, 1, 0);

    drawGrid();

    const auto& bodies = simulator.getBodies();
    for (size_t i = 0; i < bodies.size(); ++i) {
        const auto& body = bodies[i];
        float minSize = 2e9f;
        float scaleFactor = std::max(std::cbrt(body.getMass()) * 1e-9f, minSize);

        glm::vec3 pos = body.getPosition();
        std::cout << "Rendering body " << i << " (";
        switch(i) {
            case 0: std::cout << "Sun"; break;
            case 1: std::cout << "Mercury"; break;
            case 2: std::cout << "Venus"; break;
            case 3: std::cout << "Earth"; break;
            case 4: std::cout << "Mars"; break;
            default: std::cout << "Unknown"; break;
        }
        std::cout << ") at position ("
                  << pos.x << ", " << pos.y << ", " << pos.z
                  << ") with scale " << scaleFactor << std::endl;

        // Set color based on body index
        switch(i) {
            case 0: glColor3f(1.0f, 1.0f, 0.0f); break;  // Sun: Yellow
            case 1: glColor3f(0.5f, 0.5f, 0.5f); break;  // Mercury: Gray
            case 2: glColor3f(0.9f, 0.7f, 0.4f); break;  // Venus: Light Orange
            case 3: glColor3f(0.0f, 0.5f, 1.0f); break;  // Earth: Blue
            case 4: glColor3f(1.0f, 0.0f, 0.0f); break;  // Mars: Red
            default: glColor3f(1.0f, 1.0f, 1.0f); break; // White for any additional bodies
        }

        drawSphere(body.getPosition(), scaleFactor);
    }
}

bool Renderer::shouldClose() {
    return glfwWindowShouldClose(window);
}

void Renderer::swapBuffers() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Renderer::drawSphere(const glm::vec3& position, float radius) {
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glScalef(radius, radius, radius);

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glPopMatrix();
}

void Renderer::createSphereMesh(float radius, int sectors, int stacks) {
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    float x, y, z, xy;
    float nx, ny, nz, lengthInv = 1.0f / radius;
    float s, t;

    float sectorStep = 2 * M_PI / sectors;
    float stackStep = M_PI / stacks;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stacks; ++i) {
        stackAngle = M_PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectors; ++j) {
            sectorAngle = j * sectorStep;

            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);

            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    for (int i = 0; i < stacks; ++i) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);

    glBindVertexArray(sphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    glBindVertexArray(0);

    sphereVertexCount = vertices.size() / 3;
    sphereIndexCount = indices.size();
}

void Renderer::drawDebugTriangle() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.5f, 0.0f);
    glEnd();
}

void Renderer::drawGrid() {
    glBegin(GL_LINES);
    glColor3f(0.2f, 0.2f, 0.2f);  // Gray color for the grid
    for (float i = -5e11f; i <= 5e11f; i += 5e10f) {
        glVertex3f(i, 0, -5e11f);
        glVertex3f(i, 0, 5e11f);
        glVertex3f(-5e11f, 0, i);
        glVertex3f(5e11f, 0, i);
    }
    glEnd();
}