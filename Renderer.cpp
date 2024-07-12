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

Renderer::Renderer(int width, int height)
        : cameraPos(3e11f, 2e11f, 3e11f),
          cameraFront(glm::normalize(glm::vec3(0.0f) - glm::vec3(3e11f, 2e11f, 3e11f))),
          cameraUp(0.0f, 1.0f, 0.0f),
          cameraSpeed(1e9f),
          mouseSensitivity(0.05f),
          yaw(-45.0f),
          pitch(-30.0f),
          mousePressed(false),
          lastMouseX(width / 2.0),
          lastMouseY(height / 2.0)
{
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

    // Set up camera
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
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
    gluPerspective(45.0, 1600.0 / 1200.0, 1e9, 1e13);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glm::vec3 center = glm::vec3(0, 0, 0); // Look at the center of the system
    gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z,
              center.x, center.y, center.z,
              cameraUp.x, cameraUp.y, cameraUp.z);
    drawGrid(simulator);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    drawTrajectories(simulator.getBodies());
    glDisable(GL_BLEND);

    const auto& bodies = simulator.getBodies();
    double maxMass = 0;
    double minMass = std::numeric_limits<double>::max();

    // Find the maximum and minimum masses
    for (const auto& body : bodies) {
        maxMass = std::max(maxMass, body.getMass());
        minMass = std::min(minMass, body.getMass());
    }

    //std::cout << "Camera position: " << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << std::endl;
    //std::cout << "Camera front: " << cameraFront.x << ", " << cameraFront.y << ", " << cameraFront.z << std::endl;

    for (size_t i = 0; i < bodies.size(); ++i) {
        const auto& body = bodies[i];
        glm::dvec3 pos = body.getPosition();
        //std::cout << "Body " << i << " position: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
    }

    // Calculate the log range
    double logMinMass = std::log10(minMass);
    double logMaxMass = std::log10(maxMass);
    double logRange = logMaxMass - logMinMass;

    for (size_t i = 0; i < bodies.size(); ++i) {
        const auto& body = bodies[i];

        // Calculate the scale factor based on mass
        double logMass = std::log10(body.getMass());
        double normalizedLogMass = (logMass - logMinMass) / logRange;
        float minScale = 5e9f;  // Minimum scale to ensure visibility
        float maxScale = 5e10f; // Maximum scale to prevent overly large objects
        float scaleFactor = minScale + static_cast<float>(normalizedLogMass) * (maxScale - minScale);

        glm::dvec3 pos = body.getPosition();
        glm::vec3 renderPos(static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z));

//        std::cout << "Rendering body " << i << " (";
//        switch(i) {
//            case 0: std::cout << "Sun"; break;
//            case 1: std::cout << "Mercury"; break;
//            case 2: std::cout << "Venus"; break;
//            case 3: std::cout << "Earth"; break;
//            case 4: std::cout << "Mars"; break;
//            default: std::cout << "Unknown"; break;
//        }
//        std::cout << ") at position ("
//                  << pos.x << ", " << pos.y << ", " << pos.z
//                  << ") with scale " << scaleFactor << std::endl;

        // Set color based on body index
        switch(i) {
            case 0: glColor3f(1.0f, 1.0f, 0.0f); break;  // Sun: Yellow
            case 1: glColor3f(0.5f, 0.5f, 0.5f); break;  // Mercury: Gray
            case 2: glColor3f(0.9f, 0.7f, 0.4f); break;  // Venus: Light Orange
            case 3: glColor3f(0.0f, 0.5f, 1.0f); break;  // Earth: Blue
            case 4: glColor3f(1.0f, 0.0f, 0.0f); break;  // Mars: Red
            default: glColor3f(1.0f, 1.0f, 1.0f); break; // White for any additional bodies
        }

        drawSphere(renderPos, scaleFactor);
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
    gluLookAt(4e11, 3e11, 4e11, 0, 0, 0, 0, 1, 0);

    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.5f, 0.0f);
    glEnd();
}

float Renderer::calculateGravityFieldStrength(const glm::vec3& point, const std::vector<CelestialBody>& bodies) {
    float fieldStrength = 0.0f;
    const float G = 6.67430e-11f; // Gravitational constant
    const float scalingFactor = 1e20f; // Greatly increased scaling factor
    for (const auto& body : bodies) {
        glm::dvec3 bodyPos = body.getPosition();
        float distance = glm::length(glm::vec3(bodyPos) - point);
        if (distance < 1e9f) distance = 1e9f; // Prevent division by zero
        fieldStrength += scalingFactor * G * static_cast<float>(body.getMass()) / (distance * distance);
    }
    return fieldStrength;
}

void Renderer::drawGrid(const Simulator& simulator) {
    const float gridSize = 5e11f;
    const int gridLines = 20;
    const float lineSpacing = gridSize / gridLines;

    glBegin(GL_LINES);
    glColor3f(0.2f, 0.2f, 0.2f);  // Lighter gray for better visibility

    for (int i = -gridLines/2; i <= gridLines/2; ++i) {
        float pos = i * lineSpacing;
        glVertex3f(-gridSize/2, 0, pos);
        glVertex3f(gridSize/2, 0, pos);
        glVertex3f(pos, 0, -gridSize/2);
        glVertex3f(pos, 0, gridSize/2);
    }

    glEnd();
}

void Renderer::drawTrajectories(const std::vector<CelestialBody>& bodies) {
    glBegin(GL_LINES);
    for (const auto& body : bodies) {
        const auto& trajectory = body.getTrajectory();
        if (trajectory.size() < 2) continue;

        for (size_t i = 1; i < trajectory.size(); ++i) {
            glm::vec3 p1(trajectory[i-1]);
            glm::vec3 p2(trajectory[i]);

            // Fade out older parts of the trajectory
            float alpha = static_cast<float>(i) / trajectory.size();
            glColor4f(1.0f, 1.0f, 1.0f, alpha * 0.5f);

            glVertex3f(p1.x, p1.y, p1.z);
            glVertex3f(p2.x, p2.y, p2.z);
        }
    }
    glEnd();
}

void Renderer::processInput() {
    float cameraSpeed = this->cameraSpeed;

    glm::vec3 front(cameraFront.x, 0, cameraFront.z);
    front = glm::normalize(front);
    glm::vec3 right = glm::normalize(glm::cross(front, cameraUp));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += front * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= front * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= right * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += right * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void Renderer::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    renderer->handleMouseMove(xpos, ypos);
}

void Renderer::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            renderer->mousePressed = true;
            glfwGetCursorPos(window, &renderer->lastMouseX, &renderer->lastMouseY);
        } else if (action == GLFW_RELEASE) {
            renderer->mousePressed = false;
        }
    }
}

void Renderer::handleMouseMove(double xpos, double ypos) {
    if (!mousePressed) return;

    float xoffset = xpos - lastMouseX;
    float yoffset = lastMouseY - ypos;
    lastMouseX = xpos;
    lastMouseY = ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    updateCameraVectors();
}

void Renderer::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}