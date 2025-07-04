#include "../Headers/utils.h"

void framebuffer_size_callback(GLFWwindow* window, int inner_width, int inner_height)
{
    (void)window; // tmp here to remove warning BUT in future window param will be used
    glViewport(0, 0, inner_width, inner_height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) // +
        global::scale += 0.001f;
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS && global::scale > 0.f) // -
        global::scale -= 0.001f;
     
    if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
        if (!global::f3Pressed) {
            global::ShowChunkBorder = !global::ShowChunkBorder;
            global::f3Pressed = true;
        }
    }
    else {
        global::f3Pressed = false;
    }

    float speed = 0.1f;
    if (glfwGetKey(window, GLFW_KEY_J))
        global::sunX -= speed;
    if (glfwGetKey(window, GLFW_KEY_L))
        global::sunX += speed;
    if (glfwGetKey(window, GLFW_KEY_I))
        global::sunZ -= speed;
    if (glfwGetKey(window, GLFW_KEY_K))
        global::sunZ += speed;
}

void toggleFullscreen(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
        if (!global::f11Pressed) {
            global::fullscreen = !global::fullscreen;
            global::f11Pressed = true;

            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);

            if (global::fullscreen) {
                // Save current window position and size before switching
                glfwGetWindowPos(window, &global::windowed_x, &global::windowed_y);
                glfwGetWindowSize(window, &global::windowed_width, &global::windowed_height);

                glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            }
            else {
                glfwSetWindowMonitor(window, nullptr, global::windowed_x, global::windowed_y, global::windowed_width, global::windowed_height, 0);
            }
        }
    }
    else {
        global::f11Pressed = false;
    }
}

void DrawChunkBorder(float width, float height, float depth) {
    float w = width;
    float h = height;
    float d = depth;

    // 8 corners of the box
    std::vector<glm::vec3> corners = {
        {0, 0, 0}, {w, 0, 0}, {w, h, 0}, {0, h, 0},       // front face
        {0, 0, d}, {w, 0, d}, {w, h, d}, {0, h, d}        // back face
    };

    std::vector<GLuint> indices = {
        0, 1, 1, 2, 2, 3, 3, 0,       // front face
        4, 5, 5, 6, 6, 7, 7, 4,       // back face
        0, 4, 1, 5, 2, 6, 3, 7        // sides
    };

    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, corners.size() * sizeof(glm::vec3), corners.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glDrawElements(GL_LINES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
}

bool AABBInFrustum(const std::array<glm::vec4, 6>& planes, glm::vec3 min, glm::vec3 max) {
    for (const auto& plane : planes) {
        glm::vec3 normal = glm::vec3(plane);
        glm::vec3 p = min;

        if (normal.x >= 0) p.x = max.x;
        if (normal.y >= 0) p.y = max.y;
        if (normal.z >= 0) p.z = max.z;

        if (glm::dot(normal, p) + plane.w < 0) {
            return false; // Outside
        }
    }
    return true; // Inside
}

