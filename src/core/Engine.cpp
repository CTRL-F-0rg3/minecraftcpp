#include "Engine.h"
#include <iostream>

Engine::Engine(uint32_t width, uint32_t height)
: renderer(width, height),
cameraPos(0.0f, 40.0f, 50.0f),
cameraFront(0.0f, 0.0f, -1.0f),
cameraUp(0.0f, 1.0f, 0.0f),
yaw(-90.0f),
pitch(0.0f),
movementSpeed(20.0f),
mouseSensitivity(0.15f),
firstMouse(true),
lastX(0.0f),
lastY(0.0f) {
}

Engine::~Engine() {}

void Engine::run() {
    renderer.init();
    glfwSetWindowUserPointer(renderer.getWindow(), this);
    glfwSetCursorPosCallback(renderer.getWindow(), mouseCallbackStatic);
    glfwSetInputMode(renderer.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // GENERUJ 13x13 CHUNKÓW (render distance = 6)
    int renderDistance = 6;
    std::cout << "Generowanie świata (render distance: " << renderDistance << ")..." << std::endl;
    
    for (int x = -renderDistance; x <= renderDistance; x++) {
        for (int z = -renderDistance; z <= renderDistance; z++) {
            world.generateChunk(x, z);
        }
    }
    
    // Generuj meshe
    std::vector<Vertex> allVertices;
    for (int x = -renderDistance; x <= renderDistance; x++) {
        for (int z = -renderDistance; z <= renderDistance; z++) {
            auto chunkVertices = world.generateChunkMesh(x, z);
            allVertices.insert(allVertices.end(), chunkVertices.begin(), chunkVertices.end());
        }
    }
    
    std::cout << "Wygenerowano " << allVertices.size() << " wierzchołków z " 
              << (renderDistance*2+1) * (renderDistance*2+1) << " chunków" << std::endl;
    
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    
    while (!renderer.shouldClose()) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(deltaTime);
        
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 proj = glm::perspective(glm::radians(75.0f), 1280.0f / 720.0f, 0.1f, 500.0f);
        
        renderer.drawFrame(allVertices, view, proj);
    }
}

void Engine::processInput(float deltaTime) {
    glfwPollEvents();
    float velocity = movementSpeed * deltaTime;
    glm::vec3 forward = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
    glm::vec3 right = glm::normalize(glm::cross(forward, cameraUp));
    if (glfwGetKey(renderer.getWindow(), GLFW_KEY_W) == GLFW_PRESS) cameraPos += forward * velocity;
    if (glfwGetKey(renderer.getWindow(), GLFW_KEY_S) == GLFW_PRESS) cameraPos -= forward * velocity;
    if (glfwGetKey(renderer.getWindow(), GLFW_KEY_A) == GLFW_PRESS) cameraPos -= right * velocity;
    if (glfwGetKey(renderer.getWindow(), GLFW_KEY_D) == GLFW_PRESS) cameraPos += right * velocity;
    if (glfwGetKey(renderer.getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS) cameraPos += cameraUp * velocity;
    if (glfwGetKey(renderer.getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) cameraPos -= cameraUp * velocity;
    if (glfwGetKey(renderer.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(renderer.getWindow(), true);
}

void Engine::mouseCallbackStatic(GLFWwindow* window, double xpos, double ypos) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    if (engine) engine->mouseCallback(xpos, ypos);
}

void Engine::mouseCallback(double xpos, double ypos) {
    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }
    float xoffset = (xpos - lastX) * mouseSensitivity;
    float yoffset = (lastY - ypos) * mouseSensitivity;
    lastX = xpos; lastY = ypos;
    yaw += xoffset; pitch += yoffset;
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    updateCameraVectors();
}

void Engine::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}
