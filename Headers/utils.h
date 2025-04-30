#pragma once

#ifndef UTILS_H
#define UTILS_H

//#include <GLFW/glfw3.h>

#include <vector>
#include "../Headers/Camera.h"
#include "../Headers/ShaderClass.h"

void framebuffer_size_callback(GLFWwindow* window, int inner_width, int inner_height);
void processInput(GLFWwindow* window, float& inner_scale, bool& ShowChunkBorder, bool& f3Pressed);
void DrawChunkBorder(const glm::vec3& position, float width, float height, float depth, Camera& camera, Shader& shaderProgram);


#endif
