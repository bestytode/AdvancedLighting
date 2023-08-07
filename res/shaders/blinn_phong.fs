#version 330 core
out vec4 FragColor;

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D floorTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool blinn;

uniform Light light;

void main()
{
	// Ambient
	vec3 ambient = light.ambient * texture(floorTexture, TexCoords).rgb;

	// Diffuse
	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 normal = normalize(Normal);
	float diff = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse = light.diffuse * diff * texture(floorTexture, TexCoords).rgb;

	// Specular
	float spec = 0.0f;
	vec3 viewDir = normalize(viewPos - FragPos);
	if(blinn) {
		vec3 halfWayVector = normalize(lightDir + viewDir);
		spec = pow(max(dot(halfWayVector, normal), 1.0f), 32.0f);
	}
	else {
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(reflectDir, viewDir), 0.0f), 8.0f);
	}
	vec3 specular = light.specular * spec;

	// Combine
	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0f);
}