#version 330 core

in vec3 UV;

out vec4 color;

uniform samplerCube texture_sampler;

void main() {

	color = texture(texture_sampler, UV);
}
