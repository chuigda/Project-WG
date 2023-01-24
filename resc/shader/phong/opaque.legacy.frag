#version 330 core

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

const Light light = Light(vec3(1.0f), vec3(1.0f), vec3(1.0f));

in vec3 fragPos;
in vec3 normal;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
uniform vec3 light0Pos;
uniform vec3 light1Pos;

out vec4 fragColor;

void main() {
    vec3 norm = normalize(normal);

    // ambient
    vec3 ambient = light.ambient * material.ambient;

    // preparation
    vec3 viewDir = normalize(-fragPos);
    vec3 lightDir0 = normalize(light0Pos - fragPos);
    vec3 lightDir1 = normalize(light1Pos - fragPos);

    // diffuse

    // calculating with view direction instead of light direction
    float diff = max(dot(norm, viewDir), 0.0);
    vec3 diffuse = diff * material.diffuse * light.diffuse;

    // specular
    // our view point is always at the origin, thus the view direction is the negative position
    vec3 reflectDir0 = reflect(-lightDir0, norm);
    vec3 reflectDir1 = reflect(-lightDir1, norm);
    float spec0 = pow(max(dot(viewDir, reflectDir0), 0.0), material.shininess);
    float spec1 = pow(max(dot(viewDir, reflectDir1), 0.0), material.shininess);
    vec3 specular0 = light.specular * (spec0 * material.specular);
    vec3 specular1 = light.specular * (spec1 * material.specular);

    // blending
    vec3 result = ambient + diffuse + specular0 + specular1;
    fragColor = vec4(result, 1.0);
}
