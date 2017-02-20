#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

out vec3 UV;
out vec3 normal;

uniform mat4 vp;

void main(){
	gl_Position =  vp * vec4(vertexPosition_modelspace,1);
	UV = vertexPosition_modelspace;
	normal = vertexNormal_modelspace;
}
