#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	// normal lightDir and color
	vec3 normal = texture(texture_normal, TexCoords).rgb;
	normal = normalize(normal * 2.0f - 1.0f);
	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 color = texture(texture_diffuse, TexCoords).rgb;

	// Ambient
	vec3 ambient = 0.1 * color;

	// Diffuse
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * color;

	// Specular
	vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	vec3 specular = vec3(0.2) * spec;

	FragColor = vec4(ambient + diffuse + specular, 1.0);
}