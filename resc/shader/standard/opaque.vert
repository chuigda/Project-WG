#version 330 core

layout (location = 0) in vec3 inVertexCoord;
layout (location = 1) in vec3 inVertexNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBiTangent;

uniform mat4 projection;
uniform mat4 modelView;

const vec3 lightPos = vec3(0.0, 0.0, 0.0);
const vec3 viewPos = vec3(0.0, 0.0, 0.0);

out vec3 fragPos;
out vec2 texCoord;
out vec3 tangentLightPos;
out vec3 tangentViewPos;
out vec3 tangentFragPos;

void main() {
    fragPos = vec3(modelView * vec4(inVertexCoord, 1.0));
    texCoord = vec2(inTexCoord.x, 1.0 - inTexCoord.y);

    mat3 normalMatrix = transpose(inverse(mat3(modelView)));
    vec3 t = normalize(normalMatrix * inTangent);
    vec3 b = normalize(normalMatrix * inBiTangent);
    vec3 n = normalize(normalMatrix * inVertexNormal);
    mat3 tbn = mat3(t, b, n);

    tangentLightPos = tbn * lightPos;
    tangentViewPos = tbn * viewPos;
    tangentFragPos = tbn * fragPos;

    gl_Position = projection * vec4(fragPos, 1.0);
}
