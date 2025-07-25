#pragma once

#ifndef GLOBALS_H
#define GLOBALS_H

namespace global {
    extern int width;
    extern int height;

    extern int seed;

    extern int windowed_x;
    extern int windowed_y;
    extern int windowed_width;
    extern int windowed_height;

    extern float sunX;
    extern float sunY;
    extern float sunZ;

    extern float scale;

    extern float deltaTime;
    extern float lastTime;

    extern double lastFPSTime;
    extern int nbFrames;

    extern bool ShowChunkBorder;
    extern bool f3Pressed;
    extern bool fullscreen;
    extern bool f11Pressed;

    extern int renderDistance;
    extern float effectiveRadius;
    extern float squareness;

}


#endif
