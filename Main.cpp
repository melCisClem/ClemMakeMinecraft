#include <iostream>
#include <fstream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Headers/globals.h"
#include "Headers/Texture.h"
#include "Headers/ShaderClass.h"
#include "Headers/VAO.h"
#include "Headers/VBO.h"
#include "Headers/EBO.h"
#include "Headers/Camera.h"
#include "Headers/Mesh.h"
#include "Headers/utils.h"
#include "Headers/Chunk.h"

#include "Resources/Objects/cube.h"

/*
*  KeyBinds:
*   f3  -> toggle show chunk border
*   f4  -> toggle camera mode (click or immersive)
*   f11 -> toggle fullscreen (forces immersive camera mode)
*/

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    GLFWwindow* window;
    if (global::fullscreen)
        window = glfwCreateWindow(mode->width, mode->height, "ClemTest", monitor, NULL);
    else
        window = glfwCreateWindow(global::width, global::height, "ClemTest", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    gladLoadGL();

    glViewport(0, 0, global::width, global::height); // world

    std::vector<Texture> textures;
    textures.emplace_back("../Resources/Textures/atlasTest.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE);

    Shader shaderProgram("../Resources/Shaders/default.vert", "../Resources/Shaders/default.frag");
    Shader lightShader("../Resources/Shaders/light.vert", "../Resources/Shaders/light.frag");
    Shader instanceShader("../Resources/Shaders/instance.vert", "../Resources/Shaders/default.frag"); // for instancing

    Mesh light(lightVertices, lightIndices, textures);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    Camera camera(global::width, global::height, glm::vec3(8.0f, 8.0f, 30.0f));

    GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

    glm::vec4 lightColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
    glm::vec3 lightPos = glm::vec3(global::sunX, global::sunY, global::sunZ);
    glm::mat4 lightModel = glm::mat4(1.f);

    ChunkManager chunkMgr;

    for (int z = -global::renderDistance; z <= global::renderDistance; ++z)
        for (int x = -global::renderDistance; x <= global::renderDistance; ++x)
            if ((x * x + z * z) <= (global::effectiveRadius * global::effectiveRadius) / global::squareness)
                chunkMgr.initChunk(textures[0], x, z);


    while (!glfwWindowShouldClose(window)) // start of game loop
    {
        float currentTime = static_cast<float>(glfwGetTime());
        global::deltaTime = currentTime - global::lastTime;
        global::lastTime = currentTime;

        global::nbFrames++;
        if (currentTime - global::lastFPSTime >= 1.0) // If more than 1 second has passed
        {
            double fps = double(global::nbFrames);
            double frameTime = 1000.0 / fps;

            std::string title = "ClemTest - FPS: " + std::to_string(int(fps)) + " - Frame Time: " + std::to_string(frameTime).substr(0, 5) + " ms";
            glfwSetWindowTitle(window, title.c_str());

            global::nbFrames = 0;
            global::lastFPSTime = currentTime;
        }

        processInput(window);
        toggleFullscreen(window);

        lightModel = glm::translate(lightModel, lightPos);

        glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
        //glClearColor(0.07f, 0.13f, 0.17f, 1.0f); // dark gloomy black
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // uncomment to enable wireframe mode

        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 500.0f);

        lightShader.Activate();
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
        glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);

        instanceShader.Activate();
        glUniform4f(glGetUniformLocation(instanceShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
        glUniform3f(glGetUniformLocation(instanceShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform1f(uniID, global::scale);

        std::array<glm::vec4, 6> frustumPlane = camera.getFrustumPlanes();
        std::vector<glm::mat4> instanceMatrices;

        for (const auto& pair : chunkMgr.getChunks()) {
            const ChunkCoord& coord = pair.first;
            const Chunk& chunk = pair.second;

            glm::vec3 worldPos = glm::vec3(coord.x * WIDTH * global::scale, 0.0f, coord.z * DEPTH * global::scale);
            glm::vec3 aabbMin = worldPos;
            glm::vec3 aabbMax = worldPos + glm::vec3(WIDTH * global::scale, HEIGHT * global::scale, DEPTH * global::scale);

            if (!AABBInFrustum(frustumPlane, aabbMin, aabbMax))
                continue; // Skip rendering

            glm::mat4 model = glm::translate(glm::mat4(1.0f), worldPos);
            model = glm::scale(model, glm::vec3(global::scale));

            instanceMatrices.push_back(model);
        }

        // creating the instance mesh  <-- this the reason why all draws the same chunk but idk how fix
        Chunk firstChunk = chunkMgr.getChunks().begin()->second;

        Mesh instancedChunkMesh(
            firstChunk.chunkMesh.vertices,
            firstChunk.chunkMesh.indices,
            firstChunk.chunkMesh.textures,
            static_cast<unsigned int>(instanceMatrices.size()),
            instanceMatrices
        );

        instancedChunkMesh.UpdateInstances(instanceMatrices);
        instancedChunkMesh.Draw(instanceShader, camera, glm::mat4(1.0f), glm::vec3(0), glm::quat(), glm::vec3(1.0f));

        shaderProgram.Activate();

        light.Draw(lightShader, camera);

        glfwSwapBuffers(window);
        glfwPollEvents();
    } // end of game loop

    shaderProgram.Delete();
    lightShader.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


