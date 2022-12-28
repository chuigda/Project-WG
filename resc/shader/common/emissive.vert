#version 330 core

layout(location = 0) in vec3 inVertexCoord;
layout(location = 1) in vec2 inTexCoord;

uniform mat4 projection;
uniform mat4 modelView;

out vec2 texCoord;

void main() {
    gl_Position = projection * modelView * vec4(inVertexCoord, 1.0);
    texCoord = inTexCoord;
}
