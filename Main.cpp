#include <iostream>
#include <fstream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

int width = 1280;
int height = 720;
float scale = 0.25f;

float deltaTime = 0.0f;
float lastTime = 0.0f;

double lastFPSTime = 0.0;
int nbFrames = 0;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "ClemTest", NULL, NULL);
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

    glViewport(0, 0, width, height);

    Texture textures[]
    {
        Texture("../Resources/Textures/grassAtlas2.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
    };

    Shader shaderProgram("../Resources/Shaders/default.vert", "../Resources/Shaders/default.frag");
    std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
    Shader lightShader("../Resources/Shaders/light.vert", "../Resources/Shaders/light.frag");
    Mesh light(lightVertices, lightIndices, tex);

    glm::vec4 lightColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
    glm::vec3 lightPos = glm::vec3(10.f, 10.f, 10.f);
    glm::mat4 lightModel = glm::mat4(1.f);
    lightModel = glm::translate(lightModel, lightPos);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    Camera camera(width, height, glm::vec3(8.0f, 8.0f, 30.0f));

    GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

    Chunk chunk(10); // fyi: 10 here is where dirt stops and air continues NOT chunk size ( if wan change chunk size go chunk.h )
    chunk.GenerateChunk();
    chunk.BuildChunkMesh(textures[0]);

    while (!glfwWindowShouldClose(window))
    {
        float currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        nbFrames++;
        if (currentTime - lastFPSTime >= 1.0) // If more than 1 second has passed
        {
            double fps = double(nbFrames);
            double frameTime = 1000.0 / fps;

            std::string title = "ClemTest - FPS: " + std::to_string(int(fps)) + " - Frame Time: " + std::to_string(frameTime).substr(0, 5) + " ms";
            glfwSetWindowTitle(window, title.c_str());

            nbFrames = 0;
            lastFPSTime = currentTime;
        }

        processInput(window, scale);

        glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
        //glClearColor(0.07f, 0.13f, 0.17f, 1.0f); // dark gloomy black
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.Inputs(window, deltaTime);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        lightShader.Activate();
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
        glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);

        shaderProgram.Activate();

        //for (int i = 0; i < CHUNK_SIZE_X; ++i)
        //{
            glm::mat4 dirtModel = glm::mat4(1.0f);
            //dirtModel = glm::translate(dirtModel, future chunk position);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(dirtModel));
            glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
            glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

            glUniform1f(uniID, scale);
            (chunk.chunkMesh).Draw(shaderProgram, camera);
        //}


        light.Draw(lightShader, camera);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shaderProgram.Delete();
    lightShader.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


