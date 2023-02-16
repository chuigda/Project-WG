#version 330 core

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

const Light light = Light(vec3(1.0f), vec3(1.0f), vec3(1.0f));

in vec3 fragPos;
in vec2 texCoord;
in vec3 tangentLightPos;
in vec3 tangentViewPos;
in vec3 tangentFragPos;

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform Material material;
uniform sampler2D diffuseTex;
uniform sampler2D normalTex;

out vec4 fragColor;

void main() {
    vec3 normal = texture(normalTex, texCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    vec3 color = texture(diffuseTex, texCoord).rgb;
    vec3 ambient = light.ambient * vec3(material.ambient) * color;

    vec3 lightDir = normalize(tangentLightPos - tangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = light.diffuse * vec3(material.diffuse) * diff * color;

    vec3 viewDir = normalize(tangentViewPos - tangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * vec3(material.specular) * spec * color;

    fragColor = vec4(ambient + max(vec3(0.0), diffuse) + max(vec3(0.0), specular), 1.0f);
}
