cmake_minimum_required(VERSION 3.28)
project(gravity)

set(CMAKE_CXX_STANDARD 17)

# Find packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# Add executable
add_executable(gravity
        main.cpp
        CelestialBody.cpp
        Simulator.cpp
        Renderer.cpp
)

# Include directories
target_include_directories(gravity PRIVATE
        ${OPENGL_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(gravity PRIVATE
        ${OPENGL_LIBRARIES}
        GLEW::GLEW
        glfw
        glm::glm
)

# On some systems, you might need to explicitly link against OpenGL
if(UNIX AND NOT APPLE)
    target_link_libraries(gravity PRIVATE GL)
endif()