#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform float far_plane;
uniform bool shadows;

float ShadowCalculation();

void main()
{
	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 color = texture(diffuseTexture, TexCoords).rgb;
	vec3 lightColor = vec3(0.5f);

	// Ambient
	vec3 ambient = 0.3 * lightColor;

	// Diffuse
	float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

	// Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor; 
	
	// Calculate shadow
    float shadow = shadows ? ShadowCalculation() : 0.0f;                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;   

	FragColor = vec4(lighting, 1.0f);
}


float ShadowCalculation()
{
    // We use a vector from light to fragment as texture coordinates
    vec3 fragToLight = FragPos - lightPos;
    float currentDepth = length(fragToLight);

    float bias = 0.05;
    float shadow = 0.0f;
    for(float i = -0.1; i < 0.1; i += 0.05) {
        for (float j = -0.1; j < 0.1; j += 0.05) {
            for(float k = -0.1; k < 0.1; k += 0.05) {
                float closetDepth = texture(depthMap, fragToLight + vec3(i, j, k)).r;
                closetDepth *= far_plane; // Undo mapping [0, 1]
                if(currentDepth - bias > closetDepth)
                    shadow += 1.0f;
            }
        }
    }
    
    return shadow / 64;
}