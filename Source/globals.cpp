#include "../Headers/globals.h"


namespace global {
    extern int width = 1280;
    extern int height = 720;

    extern int seed = 1234567890;

    extern int windowed_x = 100, windowed_y = 100;
    extern int windowed_width = width, windowed_height = height;

    extern float scale = 0.25f;

    extern float deltaTime = 0.0f;
    extern float lastTime = 0.0f;

    extern double lastFPSTime = 0.0;
    extern int nbFrames = 0;

    extern bool ShowChunkBorder = false;
    extern bool f3Pressed = false;
    extern bool fullscreen = false;
    extern bool f11Pressed = false;

    extern int renderDistance = 8; // means player on current chunk +a chunk ard him
    extern float effectiveRadius = renderDistance + 0.5f; // idk why adding 0.5f makes it a normal circle lmao
    extern float squareness = 1.2f;  // > 1 makes corners rounder, <= 1 makes more square

}
