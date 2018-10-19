#version 330 core

out vec4 color;

in vec3 texCoords;

uniform vec3 skybox;

void main() {
    color = texture(skybox, texCoords);
}
