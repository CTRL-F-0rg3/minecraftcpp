#pragma once
#include "SubChunk.h"
#include <memory>
#include <unordered_map>

constexpr int CHUNK_HEIGHT_SUBCHUNKS = 16; // 512 / 32 = 16
constexpr int CHUNK_WORLD_HEIGHT = 512;

class Chunk {
public:
    Chunk(int x, int z) : posX(x), posZ(z) {
        for (int i = 0; i < CHUNK_HEIGHT_SUBCHUNKS; ++i) {
            subChunks[i] = std::make_unique<SubChunk>();
        }
    }

    void setBlock(int x, int y, int z, BlockType type) {
        if (y < 0 || y >= CHUNK_WORLD_HEIGHT) return;
        
        int subChunkY = y / SUBCHUNK_SIZE;
        int localY = y % SUBCHUNK_SIZE;
        
        subChunks[subChunkY]->setBlock(x, localY, z, type);
    }

    BlockType getBlock(int x, int y, int z) const {
        if (y < 0 || y >= CHUNK_WORLD_HEIGHT) return BlockType::Air;
        
        int subChunkY = y / SUBCHUNK_SIZE;
        int localY = y % SUBCHUNK_SIZE;
        
        return subChunks[subChunkY]->getBlock(x, localY, z);
    }

    SubChunk* getSubChunk(int index) {
        if (index < 0 || index >= CHUNK_HEIGHT_SUBCHUNKS) return nullptr;
        return subChunks[index].get();
    }

    int posX, posZ;

private:
    std::array<std::unique_ptr<SubChunk>, CHUNK_HEIGHT_SUBCHUNKS> subChunks;
};

// Prosty manager świata (używamy mapy hashującej dla rzadkich danych)
class World {
public:
    // Hash dla koordynatów chunka
    struct ChunkHash {
        size_t operator()(const std::pair<int, int>& p) const {
            return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
        }
    };

    void setBlock(int worldX, int worldY, int worldZ, BlockType type) {
        auto [chunkX, chunkZ] = worldToChunkCoords(worldX, worldZ);
        int localX = worldX - (chunkX * 32);
        int localZ = worldZ - (chunkZ * 32);

        auto& chunk = getOrCreateChunk(chunkX, chunkZ);
        chunk->setBlock(localX, worldY, localZ, type);
    }

    BlockType getBlock(int worldX, int worldY, int worldZ) const {
        auto [chunkX, chunkZ] = worldToChunkCoords(worldX, worldZ);
        int localX = worldX - (chunkX * 32);
        int localZ = worldZ - (chunkZ * 32);

        auto it = chunks.find({chunkX, chunkZ});
        if (it == chunks.end()) return BlockType::Air;
        
        return it->second->getBlock(localX, worldY, localZ);
    }

    Chunk* getChunk(int chunkX, int chunkZ) {
        auto it = chunks.find({chunkX, chunkZ});
        return (it != chunks.end()) ? it->second.get() : nullptr;
    }

private:
    std::pair<int, int> worldToChunkCoords(int x, int z) const {
        // Dzielenie z zaokrąglaniem w dół dla liczb ujemnych (floor division)
        int cx = (x >= 0) ? (x / 32) : ((x - 31) / 32);
        int cz = (z >= 0) ? (z / 32) : ((z - 31) / 32);
        return {cx, cz};
    }

    std::unique_ptr<Chunk>& getOrCreateChunk(int cx, int cz) {
        auto key = std::make_pair(cx, cz);
        auto it = chunks.find(key);
        if (it == chunks.end()) {
            chunks[key] = std::make_unique<Chunk>(cx, cz);
        }
        return chunks[key];
    }

    std::unordered_map<std::pair<int, int>, std::unique_ptr<Chunk>, ChunkHash> chunks;
};
