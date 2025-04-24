#include "../Headers/utils.h"

void framebuffer_size_callback(GLFWwindow* window, int inner_width, int inner_height)
{
    (void)window; // tmp here to remove warning BUT in future window param will be used
    glViewport(0, 0, inner_width, inner_height);
}

void processInput(GLFWwindow* window, float& inner_scale)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) // +
        inner_scale += 0.001f;
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS && inner_scale > 0.f) // -
        inner_scale -= 0.001f;
}

