#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform vec3 viewPos;

uniform sampler2D woodTexture;

void main()
{
	vec3 color = texture(woodTexture, TexCoords).rgb;
	vec3 normal = normalize(Normal);

	vec3 lighting = vec3(0.0f);

	for (int i = 0; i < 4; i++) {
		vec3 lightDir = normalize(lightPositions[i] - FragPos);
		float diff = max(dot(lightDir, normal), 0.0);
		vec3 diffuse = lightColors[i] * diff * color;
		vec3 result = diffuse;

		float distance = length(FragPos - lightPositions[i]);
		result *= 1.0f / (distance * distance);
		lighting += result;
	}

	FragColor = vec4(lighting, 1.0f);
}