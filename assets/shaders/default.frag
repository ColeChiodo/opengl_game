#version 330 core

out vec4 FragColor;

in vec3 currPos;
in vec3 normal;
in vec3 color;
in vec2 texCoord;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform int lightType; // 0 = Point, 1 = Directional, 2 = Spotlight
uniform vec3 lightDir;
uniform vec3 camPos;

// Phong Reflection Model
// ambient + diffuse + specular
vec4 pointLight() {
	vec3 lightVector = lightPos - currPos;
	float dist = length(lightVector);
	float a = 3.0f;
	float b = 0.7f;
	float intensity = 1.0 / (a * dist * dist + b * dist + 1.0f);

	float ambient = 0.2f;

	vec3 norm = normalize(normal);
	vec3 lightDirection = normalize(lightVector);
	float diffuse = max(dot(norm, lightDirection), 0.0f);

	float specularLight = 0.5f;
	vec3 viewDirection = normalize(camPos - currPos);
	vec3 reflectionDirection = reflect(-lightDirection, norm);
	float specularAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specularLight * specularAmount;
	
	return (texture(diffuse0, texCoord) * (diffuse * intensity + ambient) + texture(specular0, texCoord).r * specular * intensity) * lightColor;
}

vec4 directionalLight() {
	float ambient = 0.2f;

	vec3 norm = normalize(normal);
	vec3 lightDirection = normalize(lightDir);
	float diffuse = max(dot(norm, lightDirection), 0.0f);

	float specularLight = 0.5f;
	vec3 viewDirection = normalize(camPos - currPos);
	vec3 reflectionDirection = reflect(-lightDirection, norm);
	float specularAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specularLight * specularAmount;
	
	return (texture(diffuse0, texCoord) * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * lightColor;
}

vec4 spotLight() {
	float innerCone = 0.95f;
	float outerCone = 0.9f;

	float ambient = 0.2f;

	vec3 norm = normalize(normal);
	vec3 lightDirection = normalize(lightPos - currPos);
	float diffuse = max(dot(norm, lightDirection), 0.0f);

	float specularLight = 0.5f;
	vec3 viewDirection = normalize(camPos - currPos);
	vec3 reflectionDirection = reflect(-lightDirection, norm);
	float specularAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specularLight * specularAmount;

	float angle = dot(lightDir, -lightDirection);
	float intensity = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);
	
	return (texture(diffuse0, texCoord) * (diffuse * intensity + ambient) + texture(specular0, texCoord).r * specular * intensity) * lightColor;
}

void main() {
	// 0 = Point, 1 = Directional, 2 = Spotlight
	if (lightType == 0) {
        FragColor = pointLight();
    } else if (lightType == 1) {
        FragColor = directionalLight();
    } else if (lightType == 2) {
        FragColor = spotLight();
    }
}