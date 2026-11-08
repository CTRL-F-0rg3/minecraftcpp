#include "Chunk.h"
#include <cmath>
#include <algorithm>

Chunk::Chunk(int x, int z) : chunkX(x), chunkZ(z) {
    blocks.fill(BlockType::Air);
}

// PRAWDZIWY SZUM PERLIN
float Chunk::perlinNoise(float x, float z) const {
    auto hash = [](int x, int z) -> float {
        int n = x * 127 + z * 311;
        n = (n << 13) ^ n;
        return 1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f;
    };
    
    auto interpolate = [](float a, float b, float x) -> float {
        float ft = x * 3.1415927f;
        float f = (1.0f - cos(ft)) * 0.5f;
        return a * (1.0f - f) + b * f;
    };
    
    int x1 = (int)floor(x);
    int z1 = (int)floor(z);
    int x2 = x1 + 1;
    int z2 = z1 + 1;
    
    float fracX = x - x1;
    float fracZ = z - z1;
    
    float v1 = hash(x1, z1);
    float v2 = hash(x2, z1);
    float v3 = hash(x1, z2);
    float v4 = hash(x2, z2);
    
    float i1 = interpolate(v1, v2, fracX);
    float i2 = interpolate(v3, v4, fracX);
    
    return interpolate(i1, i2, fracZ);
}

float Chunk::fbm(float x, float z, int octaves, float persistence, float scale) const {
    float total = 0.0f;
    float frequency = scale;
    float amplitude = 1.0f;
    float maxValue = 0.0f;
    
    for (int i = 0; i < octaves; i++) {
        total += perlinNoise(x * frequency, z * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0f;
    }
    
    return total / maxValue;
}

void Chunk::generate() {
    blocks.fill(BlockType::Air);
    
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            int worldX = chunkX * CHUNK_SIZE + x;
            int worldZ = chunkZ * CHUNK_SIZE + z;
            
            // WIELKOSKALOWY TEREN (góry i doliny)
            float continentNoise = fbm(worldX, worldZ, 4, 0.5f, 0.005f);
            
            // ŚREDNIA SKALA (wzgórza)
            float hillsNoise = fbm(worldX, worldZ, 6, 0.5f, 0.02f);
            
            // DROBNE DETALE
            float detailNoise = fbm(worldX, worldZ, 8, 0.5f, 0.05f);
            
            // KOMBINACJA - naturalny rozkład wysokości
            float heightFloat = 20.0f + continentNoise * 30.0f + hillsNoise * 10.0f + detailNoise * 3.0f;
            int height = std::max(5, (int)heightFloat);
            
            // GENEROWANIE BLOKÓW
            for (int y = 0; y < height; y++) {
                BlockType block;
                
                if (y == height - 1) {
                    block = BlockType::Grass;  // Powierzchnia - trawa
                } else if (y > height - 4) {
                    block = BlockType::Dirt;   // Płytko - ziemia
                } else {
                    block = BlockType::Stone;  // Głęboko - kamień
                }
                
                blocks[x + y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE] = block;
            }
        }
    }
}

BlockType Chunk::getBlock(int x, int y, int z) const {
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE) return BlockType::Air;
    return blocks[x + y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE];
}

void Chunk::setBlock(int x, int y, int z, BlockType type) {
    if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_HEIGHT && z >= 0 && z < CHUNK_SIZE)
        blocks[x + y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE] = type;
}

bool Chunk::isBlockSolid(int x, int y, int z) const {
    return getBlock(x, y, z) != BlockType::Air;
}

void Chunk::addFace(std::vector<Vertex>& vertices, int x, int y, int z, int face, BlockType block) {
    // POZYCJA ŚWIATA - każdy chunk ma swój offset
    float worldX = chunkX * CHUNK_SIZE;
    float worldZ = chunkZ * CHUNK_SIZE;
    
    float px = worldX + (float)x;
    float py = (float)y;
    float pz = worldZ + (float)z;
    
    // Cieniowanie ścian
    float colors[6][3] = {
        {0.8f, 0.8f, 0.8f},  // +X
        {0.8f, 0.8f, 0.8f},  // -X
        {1.0f, 1.0f, 1.0f},  // +Y (góra)
        {0.5f, 0.5f, 0.5f},  // -Y (dół)
        {0.9f, 0.9f, 0.9f},  // +Z
        {0.9f, 0.9f, 0.9f}   // -Z
    };
    float* c = colors[face];
    
    auto addV = [&](float vx, float vy, float vz, float u, float v) {
        vertices.push_back({{px+vx, py+vy, pz+vz}, {c[0], c[1], c[2]}, {u, v}, (float)block});
    };
    
    switch(face) {
        case 0:
            addV(1, 0, 0, 0, 0); addV(1, 1, 0, 0, 1); addV(1, 1, 1, 1, 1);
            addV(1, 0, 0, 0, 0); addV(1, 1, 1, 1, 1); addV(1, 0, 1, 1, 0); break;
        case 1:
            addV(0, 0, 1, 0, 0); addV(0, 1, 1, 0, 1); addV(0, 1, 0, 1, 1);
            addV(0, 0, 1, 0, 0); addV(0, 1, 0, 1, 1); addV(0, 0, 0, 1, 0); break;
        case 2:
            addV(0, 1, 0, 0, 0); addV(1, 1, 1, 1, 1); addV(1, 1, 0, 1, 0);
            addV(0, 1, 0, 0, 0); addV(0, 1, 1, 0, 1); addV(1, 1, 1, 1, 1); break;
        case 3:
            addV(0, 0, 1, 0, 0); addV(1, 0, 0, 1, 1); addV(1, 0, 1, 1, 0);
            addV(0, 0, 1, 0, 0); addV(0, 0, 0, 0, 1); addV(1, 0, 0, 1, 1); break;
        case 4:
            addV(1, 0, 1, 0, 0); addV(1, 1, 1, 0, 1); addV(0, 1, 1, 1, 1);
            addV(1, 0, 1, 0, 0); addV(0, 1, 1, 1, 1); addV(0, 0, 1, 1, 0); break;
        case 5:
            addV(0, 0, 0, 0, 0); addV(0, 1, 0, 0, 1); addV(1, 1, 0, 1, 1);
            addV(0, 0, 0, 0, 0); addV(1, 1, 0, 1, 1); addV(1, 0, 0, 1, 0); break;
    }
}

std::vector<Vertex> Chunk::generateMesh() {
    std::vector<Vertex> vertices;
    for (int y = 0; y < CHUNK_HEIGHT; y++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                BlockType block = getBlock(x, y, z);
                if (block == BlockType::Air) continue;
                if (!isBlockSolid(x+1, y, z)) addFace(vertices, x, y, z, 0, block);
                if (!isBlockSolid(x-1, y, z)) addFace(vertices, x, y, z, 1, block);
                if (!isBlockSolid(x, y+1, z)) addFace(vertices, x, y, z, 2, block);
                if (!isBlockSolid(x, y-1, z)) addFace(vertices, x, y, z, 3, block);
                if (!isBlockSolid(x, y, z+1)) addFace(vertices, x, y, z, 4, block);
                if (!isBlockSolid(x, y, z-1)) addFace(vertices, x, y, z, 5, block);
            }
        }
    }
    return vertices;
}
