#include "../Headers/Camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	view = glm::lookAt(Position, Position + Orientation, Up);
	projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

	cameraMatrix = projection * view;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::Inputs(GLFWwindow* window)
{
    float camSpeed = speed * 10.f * global::deltaTime;

    // Toggle camera mode on F4 
    if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS && !f4PressedLastFrame)
    {
        cameraControlActive = !cameraControlActive;
        f4PressedLastFrame = true;
    }
    if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_RELEASE)
    {
        f4PressedLastFrame = false;
    }

    if (global::fullscreen) cameraControlActive = true;

    // Movement control
    if (cameraControlActive || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        if (firstClick)
        {
            glfwSetCursorPos(window, width / 2, height / 2);
            firstClick = false;
        }

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float rotX = sensitivity * (float)(mouseY - height / 2) / height;
        float rotY = sensitivity * (float)(mouseX - width / 2) / width;

        glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));
        if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
            Orientation = newOrientation;

        Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);
        glfwSetCursorPos(window, width / 2, height / 2);

        // Movement
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) Position += camSpeed * Orientation;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) Position += camSpeed * -glm::normalize(glm::cross(Orientation, Up));
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) Position += camSpeed * -Orientation;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) Position += camSpeed * glm::normalize(glm::cross(Orientation, Up));
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) Position += camSpeed * Up;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) Position += camSpeed * -Up;
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstClick = true;
    }

    // Speed boost
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) speed = 3.f; // basically 3X the normal speed
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) speed = 1.f;
}

std::array<glm::vec4, 6> Camera::getFrustumPlanes() const {
    std::array<glm::vec4, 6> planes;
    glm::mat4 matrix = cameraMatrix;

    // Left
    planes[0] = glm::vec4(matrix[0][3] + matrix[0][0],
        matrix[1][3] + matrix[1][0],
        matrix[2][3] + matrix[2][0],
        matrix[3][3] + matrix[3][0]);
    // Right
    planes[1] = glm::vec4(matrix[0][3] - matrix[0][0],
        matrix[1][3] - matrix[1][0],
        matrix[2][3] - matrix[2][0],
        matrix[3][3] - matrix[3][0]);
    // Bottom
    planes[2] = glm::vec4(matrix[0][3] + matrix[0][1],
        matrix[1][3] + matrix[1][1],
        matrix[2][3] + matrix[2][1],
        matrix[3][3] + matrix[3][1]);
    // Top
    planes[3] = glm::vec4(matrix[0][3] - matrix[0][1],
        matrix[1][3] - matrix[1][1],
        matrix[2][3] - matrix[2][1],
        matrix[3][3] - matrix[3][1]);
    // Near
    planes[4] = glm::vec4(matrix[0][3] + matrix[0][2],
        matrix[1][3] + matrix[1][2],
        matrix[2][3] + matrix[2][2],
        matrix[3][3] + matrix[3][2]);
    // Far
    planes[5] = glm::vec4(matrix[0][3] - matrix[0][2],
        matrix[1][3] - matrix[1][2],
        matrix[2][3] - matrix[2][2],
        matrix[3][3] - matrix[3][2]);

    // Normalize
    for (auto& plane : planes) {
        float length = glm::length(glm::vec3(plane));
        plane /= length;
    }

    return planes;
}
