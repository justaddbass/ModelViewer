#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

out vec2 UV;
out vec3 normal;

uniform mat4 vp;

void main() {
	gl_Position =  vp * vec4(0.5 * vertexPosition_modelspace,1);
	UV = vertexUV;
	normal = vertexNormal_modelspace;
}
