#pragma once

#ifndef CHUNK_H
#define CHUNK_H

#include "../Headers/Mesh.h"

const int CHUNK_SIZE_X = 16, CHUNK_SIZE_Y = 16, CHUNK_SIZE_Z = 16;

enum BlockType { Air = 0, Dirt, Grass };

class Chunk {
	int Dirt_StopY; // where dirt stops and air starts
	BlockType chunk[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z];

public:
	Mesh chunkMesh;

	Chunk(int Dirt_StopY); // Ctor
	~Chunk(); //Dtor

	void GenerateChunk();
	bool isAir(int x, int y, int z);
	glm::vec2 GetUV(BlockType type, int face, int cornerX, int cornerY);
	void AddFace(std::vector<Vertex>& verts, std::vector<GLuint>& inds, int x, int y, int z, glm::vec3 normal, glm::vec3 quad[4], BlockType type, int faceID, GLuint& indexOffset);
	void BuildChunkMesh(Texture& atlasTexture);
};

#endif
