#version 330 core

layout (location = 0) in vec3 inVertexCoord;
layout (location = 1) in vec3 inVertexNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBiTangent;

uniform mat4 projection;
uniform mat4 modelView;

out vec3 fragPos;
out vec2 texCoord;

void main() {
    fragPos = vec3(modelView * vec4(inVertexCoord, 1.0));
    texCoord = vec2(inTexCoord.x, 1.0 - inTexCoord.y);

    gl_Position = projection * vec4(fragPos, 1.0);
}
