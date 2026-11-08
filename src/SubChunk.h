#pragma once
#include <array>
#include <cstdint>
#include <vector>

// Typy bloków (na razie tylko baza)
enum class BlockType : uint16_t {
    Air = 0,
    Stone = 1,
    Dirt = 2,
    Grass = 3,
    // ...
};

constexpr int SUBCHUNK_SIZE = 32;
constexpr int SUBCHUNK_VOLUME = SUBCHUNK_SIZE * SUBCHUNK_SIZE * SUBCHUNK_SIZE; // 32768

class SubChunk {
public:
    SubChunk() {
        // Domyślnie wypełniamy powietrzem
        blocks.fill(static_cast<uint16_t>(BlockType::Air));
    }

    // Szybkie indeksowanie 3D -> 1D (X + Z*32 + Y*1024)
    // Układ: X jest ciągły, potem Z, potem Y.
    inline size_t getIndex(int x, int y, int z) const {
        return x + (z << 5) + (y << 10);
    }

    void setBlock(int x, int y, int z, BlockType type) {
        blocks[getIndex(x, y, z)] = static_cast<uint16_t>(type);
        isDirty = true; // Flaga do przeliczenia meshu
    }

    BlockType getBlock(int x, int y, int z) const {
        return static_cast<BlockType>(blocks[getIndex(x, y, z)]);
    }

    bool needsRemesh() const { return isDirty; }
    void clearDirtyFlag() { isDirty = false; }

    // Surowe dane do wrzucenia do bufora Vulkanowego (np. przy meshowaniu)
    const std::array<uint16_t, SUBCHUNK_VOLUME>& getRawData() const { return blocks; }

private:
    std::array<uint16_t, SUBCHUNK_VOLUME> blocks;
    bool isDirty = true;
};
