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
uniform int lightType;
uniform vec3 lightDir;
uniform vec3 camPos;
uniform bool useVertexColor;

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
	float specularAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 32);
	float specular = specularLight * specularAmount;

	vec4 baseColor = useVertexColor ? vec4(color, 1.0) : texture(diffuse0, texCoord);
	float specMap = useVertexColor ? 0.0 : texture(specular0, texCoord).r;

	return (baseColor * (diffuse * intensity + ambient) + specMap * specular * intensity) * lightColor;
}

vec4 directionalLight() {
	float ambient = 0.2f;

	vec3 norm = normalize(normal);
	vec3 lightDirection = normalize(lightDir * vec3(-1.0f, -1.0f, -1.0f));
	float diffuse = max(dot(norm, lightDirection), 0.0f);

	float specularLight = 0.5f;
	vec3 viewDirection = normalize(camPos - currPos);
	vec3 reflectionDirection = reflect(-lightDirection, norm);
	float specularAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 32);
	float specular = specularLight * specularAmount;

	vec4 baseColor = useVertexColor ? vec4(color, 1.0) : texture(diffuse0, texCoord);
	float specMap = useVertexColor ? 0.0 : texture(specular0, texCoord).r;

	return (baseColor * (diffuse + ambient) + specMap * specular) * lightColor;
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
	float specularAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 32);
	float specular = specularLight * specularAmount;

	float angle = dot(lightDir, -lightDirection);
	float intensity = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	vec4 baseColor = useVertexColor ? vec4(color, 1.0) : texture(diffuse0, texCoord);
	float specMap = useVertexColor ? 0.0 : texture(specular0, texCoord).r;

	return (baseColor * (diffuse * intensity + ambient) + specMap * specular * intensity) * lightColor;
}

void main() {
	if (lightType == 0) {
        FragColor = pointLight();
    } else if (lightType == 1) {
        FragColor = directionalLight();
    } else if (lightType == 2) {
        FragColor = spotLight();
    }
}
