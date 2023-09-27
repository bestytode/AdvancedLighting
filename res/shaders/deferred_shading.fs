#version 330 core
#define NR_LIGHTS 32
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
	vec3 Position;
	vec3 Color;

	float Linear;
	float Quadratic;
};

uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main()
{
	// Retrieve g-Buffer data
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
	vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
	float Specular = texture(gAlbedoSpec, TexCoords).a;

	// Calculate lighting
	vec3 lighting = vec3(0.1f) * Diffuse;
	vec3 viewDir = normalize(viewPos - FragPos);
	for(int i = 0; i < NR_LIGHTS; i++) {
		vec3 lightDir = normalize(lights[i].Position - FragPos);

		// Diffuse
		vec3 diffuse = lights[i].Color * Diffuse * max(dot(Normal, lightDir), 0.0f) * 1.5f;
        // Specular
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(halfwayDir, Normal), 0.0f), 32.0f);
		vec3 specular = lights[i].Color * spec * Specular;
		// Attenuation
		float distance = length(lights[i].Position - FragPos);
		float attenuation = 1.0f / (1.0f + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
		diffuse *= attenuation;
		specular *= attenuation;
		lighting += diffuse + specular;
	}

	FragColor = vec4(lighting, 1.0f);
}