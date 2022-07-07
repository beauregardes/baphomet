#version 330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 inTexCoord;
layout(location = 2) in vec4 inFgColor;

out vec3 texCoord;
out vec4 fgColor;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(vertPos, 1);
    texCoord = inTexCoord;
    fgColor = inFgColor;
}