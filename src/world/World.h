#pragma once
#include "Chunk.h"
#include <unordered_map>
#include <memory>

class World {
public:
    World();
    
    void generateChunk(int chunkX, int chunkZ);
    Chunk* getChunk(int chunkX, int chunkZ);
    
    std::vector<Vertex> generateChunkMesh(int chunkX, int chunkZ);

private:
    std::unordered_map<int64_t, std::unique_ptr<Chunk>> chunks;
    
    int64_t getChunkKey(int x, int z) const;
};
