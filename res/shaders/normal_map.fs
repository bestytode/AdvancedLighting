#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 tangentLightPos;
in vec3 tangentViewPos;
in vec3 tangentFragPos;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform float lightIntensity;

void main()
{
	// Fetch and normalize the normal from the normal map
	vec3 normal = texture(texture_normal, TexCoords).rgb;
	normal = normalize(normal * 2.0f - 1.0f);

	// Compute the light direction in tangent space
	vec3 lightDir = normalize(tangentLightPos - tangentFragPos);

	// Compute the view direction in tangent space
	vec3 viewDir = normalize(tangentViewPos - tangentFragPos);

	// Fetch the color from the diffuse map
	vec3 color = texture(texture_diffuse, TexCoords).rgb;

	// Compute ambient, diffuse, and specular components
	vec3 ambient = 0.1 * color;
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * color;
	vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	vec3 specular = vec3(0.2) * spec;

	vec3 lighting = ambient + lightIntensity * (diffuse + specular);
	FragColor = vec4(lighting, 1.0);
}