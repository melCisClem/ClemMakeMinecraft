#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;
layout (location = 4) in mat4 instanceMatrix;

out vec3 crntPos;
out vec3 Normal;
out vec3 color;
out vec2 texCoord;

uniform mat4 camMatrix;
uniform float scale;

void main()
{
    vec4 worldPos = instanceMatrix * vec4(aPos * scale, 1.0f);
    crntPos = worldPos.xyz;
    Normal = mat3(transpose(inverse(mat3(instanceMatrix)))) * aNormal;
    color = aColor;
    texCoord = aTex;

    gl_Position = camMatrix * worldPos;
}