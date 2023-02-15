#version 330 core

in vec3 fragPos;
in vec2 texCoord;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
uniform sampler2D diffuseTex;
uniform sampler2D normalTex;

out vec4 fragColor;

void main() {
    fragColor = texture(diffuseTex, texCoord);
}
