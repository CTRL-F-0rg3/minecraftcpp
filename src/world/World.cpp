#include "World.h"

World::World() {}

int64_t World::getChunkKey(int x, int z) const {
    return ((int64_t)x << 32) | (int64_t)(z & 0xFFFFFFFF);
}

void World::generateChunk(int chunkX, int chunkZ) {
    int64_t key = getChunkKey(chunkX, chunkZ);
    if (chunks.find(key) == chunks.end()) {
        chunks[key] = std::make_unique<Chunk>(chunkX, chunkZ);
        chunks[key]->generate();
    }
}

Chunk* World::getChunk(int chunkX, int chunkZ) {
    int64_t key = getChunkKey(chunkX, chunkZ);
    auto it = chunks.find(key);
    if (it != chunks.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::vector<Vertex> World::generateChunkMesh(int chunkX, int chunkZ) {
    Chunk* chunk = getChunk(chunkX, chunkZ);
    if (chunk) {
        return chunk->generateMesh();
    }
    return {};
}
