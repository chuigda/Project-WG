#version 330 core

layout(location = 0) in vec3 inVertexCoord;
layout(location = 1) in vec3 inNormal;

uniform mat4 projection;
uniform mat4 modelView;

out vec3 fragPos;
out vec3 normal;

void main() {
    fragPos = vec3(modelView * vec4(inVertexCoord, 1.0));
    normal = mat3(transpose(inverse(modelView))) * inNormal;

    gl_Position = projection * vec4(fragPos, 1.0);
}
