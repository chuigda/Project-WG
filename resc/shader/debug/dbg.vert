#version 330 core

layout (location = 0) in vec3 inVertexCoord;
layout (location = 1) in vec3 inVertexNormal;

uniform mat4 projection;
uniform mat4 modelView;

void main() {
    gl_Position = projection * modelView * vec4(inVertexCoord, 1.0);
}
