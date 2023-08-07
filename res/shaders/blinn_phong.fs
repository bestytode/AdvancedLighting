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
    // Properties
	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(lightPos - FragPos);

	// Ambient
	vec3 ambient = light.ambient * texture(floorTexture, TexCoords).rgb;

	// Diffuse
	float diff = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = light.diffuse * diff * texture(floorTexture, TexCoords).rgb;

	// Specular
	float spec = 0.0f;
	if(blinn) {
		vec3 reflect = reflect(-lightDir, normal);
		spec = pow(max(dot(reflect, viewDir), 0.0f), 32.0f);
	}
	else {
		vec3 halfWayVector = normalize(viewDir + lightDir);
		spec = pow(max(dot(normal, halfWayVector), 0.0f), 8.0f);
	}
	vec3 specular = light.specular * spec; // Assuming bright white light

	// Combine
	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0f);
}