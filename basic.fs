#version 330 core

in vec2 UV;
in vec3 normal;
in vec3 fragPos;

out vec4 color;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D texture_sampler;

void main() {
	float ambientStrength = 0.1f;
	float specularStrength = 0.6f;
	vec3 lightColor = vec3(1.0, 1.0, 1.0);

	// diffuse
	vec3 lightDir = normalize(lightPos - fragPos);
	vec3 diff = max(dot(normal, lightDir), 0.0) * lightColor;

	// Specular
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	color = vec4((ambientStrength + diff + specular)/* * normal.xyz*/, 1.0);
	//color = texture(texture_sampler, UV);
}
