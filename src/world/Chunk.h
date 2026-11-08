#pragma once
#include "core/Vertex.h"
#include <array>
#include <cstdint>
#include <vector>

constexpr int CHUNK_SIZE = 16;
constexpr int CHUNK_HEIGHT = 64;

enum class BlockType : uint8_t {
    Air = 0, Stone = 1, Dirt = 2, Grass = 3, Sand = 4
};

class Chunk {
public:
    Chunk(int x, int z);
    void generate();
    std::vector<Vertex> generateMesh();
    BlockType getBlock(int x, int y, int z) const;
    void setBlock(int x, int y, int z, BlockType type);
    int chunkX, chunkZ;
private:
    std::array<BlockType, CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE> blocks;
    float perlinNoise(float x, float z) const;
    float fbm(float x, float z, int octaves, float persistence, float scale) const;
    bool isBlockSolid(int x, int y, int z) const;
    void addFace(std::vector<Vertex>& vertices, int x, int y, int z, int face, BlockType block);
};
