#version 330 core

in vec2 texCoord;

uniform sampler2D screenTexture;

out vec4 fragColor;

void main() {
    vec4 color = texture(screenTexture, texCoord);
    fragColor = color;
}
