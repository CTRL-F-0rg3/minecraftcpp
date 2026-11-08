#pragma once
#include <glm/glm.hpp>

struct Vertex {
    float pos[3];
    float color[3];
    float uv[2];
    float blockType; // 1=stone, 2=dirt, 3=grass
};
