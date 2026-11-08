#pragma once
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "renderer/Renderer.h"
#include "world/World.h"


#include <unordered_map>

class Engine {
public:
    Engine(uint32_t width, uint32_t height);
    ~Engine();
    void run();
private:
    Renderer renderer;
    World world;
    glm::vec3 cameraPos, cameraFront, cameraUp;
    float yaw, pitch, movementSpeed, mouseSensitivity, lastX, lastY;
    bool firstMouse;
    int renderDistance;
    std::unordered_map<int64_t, std::vector<Vertex>> meshCache;
    int64_t chunkKey(int x, int z) { return ((int64_t)x << 32) | (int64_t)(z & 0xFFFFFFFF); }
    void processInput(float deltaTime);
    void updateCameraVectors();
    static void mouseCallbackStatic(GLFWwindow* window, double xpos, double ypos);
    void mouseCallback(double xpos, double ypos);
};
