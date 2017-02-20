#version 330 core

in vec3 UV;
in vec3 normal;

out vec4 color;

uniform samplerCube texture_sampler;

void main() {

	color = vec4(normal.x, normal.y, normal.z, 1.0);
}
