#pragma once

#ifndef UTILS_H
#define UTILS_H

//#include <GLFW/glfw3.h>

#include <vector>
#include "../Headers/Camera.h"
#include "../Headers/ShaderClass.h"
#include "../Headers/globals.h"

void framebuffer_size_callback(GLFWwindow* window, int inner_width, int inner_height);
void processInput(GLFWwindow* window);
void toggleFullscreen(GLFWwindow* window);

void DrawChunkBorder(float width, float height, float depth);
bool AABBInFrustum(const std::array<glm::vec4, 6>& planes, glm::vec3 min, glm::vec3 max);

#endif
