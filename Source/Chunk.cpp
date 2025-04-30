#include "../Headers/Chunk.h"


Chunk::Chunk(int Dirt_StopY) : Dirt_StopY{ Dirt_StopY } 
{
    for (int x = 0; x < WIDTH; x++)
        for (int y = 0; y < HEIGHT; y++)
            for (int z = 0; z < DEPTH; z++)
            {
                int index = x + WIDTH * (y + DEPTH * z);
                if (y < Dirt_StopY - 1)
                    chunk[index] = Dirt;
                else if (y == Dirt_StopY - 1)
                    chunk[index] = Grass;
                else
                    chunk[index] = Air;
            }
}

Chunk::~Chunk() {};

BlockType Chunk::getBlock(int x, int y, int z) const 
{
    return chunk[x + WIDTH * (y + HEIGHT * z)];
}

void Chunk::setBlock(int x, int y, int z, BlockType type) 
{
    chunk[x + WIDTH * (y + HEIGHT * z)] = type;
}

bool Chunk::isAir(int x, int y, int z)
{
    if (x < 0 || y < 0 || z < 0 || x >= WIDTH || y >= HEIGHT || z >= DEPTH)
        return true;
    return chunk[x + WIDTH * (y + DEPTH * z)] == Air;
}

glm::vec2 Chunk::GetUV(BlockType type, int face, int cornerX, int cornerY)
{
    glm::ivec2 tile = { 0, 0 }; // Texture positions per block type and face

    switch (type)
    {
    case Grass:
        switch (face) // 0 1 2 3 -> left front right back  4 5 -> top bottom
        {
        case 0: tile = { 0, 0 }; break; // left
        case 1: tile = { 0, 0 }; break; // front
        case 2: tile = { 0, 0 }; break; // right
        case 3: tile = { 0, 0 }; break; // back
        case 4: tile = { 4, 0 }; break; // top
        case 5: tile = { 5, 0 }; break; // bottom
        }
        break;

    case Dirt:
        tile = { 5, 0 }; // dirt
        break;

    default:
        tile = { 0, 0 }; // Air 
        break;
    }

    const float padding = 0.002f;

    float atlasTilesX = 6.0f; // amt of tiles horizontally
    float atlasTilesY = 1.0f; // amt of tiles vertically

    float tileSizeX = 1.0f / atlasTilesX;
    float tileSizeY = 1.0f / atlasTilesY;

    float u = tile.x * tileSizeX + cornerX * (tileSizeX - 2 * padding) / 1.0f + padding;
    float v = tile.y * tileSizeY + cornerY * (tileSizeY - 2 * padding) / 1.0f + padding;

    return glm::vec2(u, v);
}

void Chunk::AddFace(std::vector<Vertex>& verts, std::vector<GLuint>& inds, int x, int y, int z, glm::vec3 normal, glm::vec3 quad[4], BlockType type, int faceID, GLuint& indexOffset)
{
    const int amtOfTex = 4;

    glm::ivec2 uvMap[4] = {
        {0, 0}, // bottom-left
        {0, 1}, // top-left
        {1, 1}, // top-right
        {1, 0}  // bottom-right
    };

    for (int i = 0; i < 4; i++)
    {
        Vertex v;
        v.position = glm::vec3(x, y, z) + quad[i];
        v.normal = normal;
        v.color = glm::vec3(1.0f);
        v.texUV = GetUV(type, faceID, uvMap[i].x, uvMap[i].y);
        verts.push_back(v);
    }

    inds.insert(inds.end(), {
        indexOffset + 0, indexOffset + 1, indexOffset + 2,
        indexOffset + 0, indexOffset + 2, indexOffset + 3
        });

    indexOffset += amtOfTex;
}

void Chunk::BuildChunkMesh(Texture& atlasTexture)
{
    std::vector<Vertex> verts;
    std::vector<GLuint> inds;
    GLuint indexOffset = 0;

    glm::vec3 frontFace[] = { {0,0,1}, {0,1,1}, {1,1,1}, {1,0,1} };
    glm::vec3 backFace[] = { {1,0,0}, {1,1,0}, {0,1,0}, {0,0,0} };
    glm::vec3 leftFace[] = { {0,0,0}, {0,1,0}, {0,1,1}, {0,0,1} };
    glm::vec3 rightFace[] = { {1,0,1}, {1,1,1}, {1,1,0}, {1,0,0} };
    glm::vec3 topFace[] = { {0,1,1}, {0,1,0}, {1,1,0}, {1,1,1} };
    glm::vec3 bottomFace[] = { {0,0,0}, {0,0,1}, {1,0,1}, {1,0,0} };

    for (int x = 0; x < WIDTH; x++)
        for (int y = 0; y < HEIGHT; y++)
            for (int z = 0; z < DEPTH; z++)
            {
                int index = x + WIDTH * (y + HEIGHT * z);
                BlockType type = chunk[index];
                if (type == Air) continue;

                BlockType visibleType = type;

                // If there's a block above and is grass block, show dirt sides instead 
                if (!isAir(x, y + 1, z))
                    visibleType = Dirt;

                // Only draw faces that are next to air
                if (isAir(x, y, z + 1)) AddFace(verts, inds, x, y, z, { 0,0,1 }, frontFace, visibleType, 1, indexOffset);   // Front
                if (isAir(x, y, z - 1)) AddFace(verts, inds, x, y, z, { 0,0,-1 }, backFace, visibleType, 3, indexOffset);   // Back
                if (isAir(x - 1, y, z)) AddFace(verts, inds, x, y, z, { -1,0,0 }, leftFace, visibleType, 0, indexOffset);   // Left
                if (isAir(x + 1, y, z)) AddFace(verts, inds, x, y, z, { 1,0,0 }, rightFace, visibleType, 2, indexOffset);   // Right
                if (isAir(x, y + 1, z)) AddFace(verts, inds, x, y, z, { 0,1,0 }, topFace, visibleType, 4, indexOffset);     // Top
                if (isAir(x, y - 1, z)) AddFace(verts, inds, x, y, z, { 0,-1,0 }, bottomFace, visibleType, 5, indexOffset); // Bottom

            }

    std::vector<Texture> textures = { atlasTexture };
    chunkMesh = Mesh(verts, inds, textures);
}


ChunkManager::ChunkManager() { chunks.reserve(16); }

ChunkManager::ChunkManager(int renderDist) { chunks.reserve(renderDist); };

ChunkManager::~ChunkManager() {};

void ChunkManager::initChunk(Texture& tex, int chunkX, int chunkZ, int Dirt_StopY) {
    ChunkCoord coord{ chunkX, chunkZ };

    // Prevent overwriting existing chunks
    if (chunks.find(coord) != chunks.end()) return;

    Chunk chunk(Dirt_StopY);
    chunk.BuildChunkMesh(tex);
    chunks.emplace(coord, std::move(chunk));
}

const std::unordered_map<ChunkCoord, Chunk>& ChunkManager::getChunks() const { return chunks; }

