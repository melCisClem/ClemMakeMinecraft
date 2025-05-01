#pragma once

#ifndef CHUNK_H
#define CHUNK_H

#include "../Headers/Mesh.h"

#include "stb/stb_perlin.h""

#include <vector>
#include <unordered_map>

static const int WIDTH = 16; // x
static const int HEIGHT = 16; // y
static const int DEPTH = 16; // z

static const int CHUNK_SIZE = WIDTH * HEIGHT * DEPTH;

enum BlockType { Air = 0, Dirt, Grass };

class Chunk {
	BlockType chunk[CHUNK_SIZE]; // index = x + WIDTH * (y + HEIGHT * z)

public:
	Mesh chunkMesh;

	Chunk(int chunkX, int chunkZ); // Ctor
	~Chunk(); //Dtor

	BlockType getBlock(int x, int y, int z) const;
	void setBlock(int x, int y, int z, BlockType type);

	bool isAir(int x, int y, int z);
	glm::vec2 GetUV(BlockType type, int face, int cornerX, int cornerY);
	void AddFace(std::vector<Vertex>& verts, std::vector<GLuint>& inds, int x, int y, int z, glm::vec3 normal, glm::vec3 quad[4], BlockType type, int faceID, GLuint& indexOffset);
	void BuildChunkMesh(Texture& atlasTexture);
};

struct ChunkCoord {
	int x, z;

	bool operator==(const ChunkCoord& other) const {
		return x == other.x && z == other.z;
	}
};

namespace std {
	template <>
	struct hash<ChunkCoord> {
		std::size_t operator()(const ChunkCoord& coord) const noexcept { // for hash table
			return std::hash<int>()(coord.x) ^ (std::hash<int>()(coord.z) << 1);
		}
	};
}

class ChunkManager {
	std::unordered_map<ChunkCoord, Chunk> chunks;
public:
	ChunkManager();
	ChunkManager(int renderDist);
	~ChunkManager();

	void initChunk(Texture& tex, int chunkX, int chunkZ);

	const std::unordered_map<ChunkCoord, Chunk>& getChunks() const;
};

#endif
