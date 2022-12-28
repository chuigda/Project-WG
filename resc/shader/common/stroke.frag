#version 330 core

uniform vec3 strokeColor;

out vec4 fragColor;

void main() {
    fragColor = vec4(strokeColor, 1.0f);
}
