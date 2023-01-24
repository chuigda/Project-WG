#version 330 core

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

const Light light = Light(vec3(1.0f), vec3(1.0f), vec3(1.0f));

layout (location = 0) in vec3 inVertexCoord;
layout (location = 1) in vec3 inNormal;

uniform mat4 projection;
uniform mat4 modelView;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
uniform vec3 light0Pos;
uniform vec3 light1Pos;

out vec3 color;

void main() {
    vec3 fragPos = vec3(modelView * vec4(inVertexCoord, 1.0));
    vec3 normal = mat3(transpose(inverse(modelView))) * inNormal;
    vec3 norm = normalize(normal);

    // ambient
    vec3 ambient = light.ambient * material.ambient;

    // diffuse
    vec3 lightDir0 = normalize(light0Pos - fragPos);
    vec3 lightDir1 = normalize(light1Pos - fragPos);
    float diff0 = max(dot(norm, lightDir0), 0.0);
    float diff1 = max(dot(norm, lightDir1), 0.0);
    vec3 diffuse0 = diff0 * material.diffuse * light.diffuse;
    vec3 diffuse1 = diff1 * material.diffuse * light.diffuse;

    // specular
    // our view point is always at the origin, thus the view direction is the negative position
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir0 = reflect(-lightDir0, norm);
    vec3 reflectDir1 = reflect(-lightDir1, norm);
    float spec0 = pow(max(dot(viewDir, reflectDir0), 0.0), material.shininess);
    float spec1 = pow(max(dot(viewDir, reflectDir1), 0.0), material.shininess);
    vec3 specular0 = light.specular * (spec0 * material.specular);
    vec3 specular1 = light.specular * (spec1 * material.specular);

    // blending
    color = ambient + diffuse0 + diffuse1 + specular0 + specular1;

    gl_Position = projection * vec4(fragPos, 1.0);
}
