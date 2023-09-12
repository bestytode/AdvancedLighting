#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 tangentLightPos;
in vec3 tangentViewPos;
in vec3 tangentFragPos;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float height_scale;
uniform float lightIntensity;

// Attenuation coefficients
uniform float constant;
uniform float linear;
uniform float quadratic;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

void main()
{
	vec3 viewDir = normalize(tangentViewPos - tangentFragPos);

	vec2 texCoords = ParallaxMapping(TexCoords, viewDir);
    if (texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0) 
        discard;

	// normal
    vec3 normal = texture(normalMap, texCoords).rgb;
	normal = normalize(normal * 2.0f - 1.0f);

	// Get diffuse color
    vec3 color = texture(diffuseMap, texCoords).rgb;
    vec3 ambient = 0.1 * color;
    vec3 lightDir = normalize(tangentLightPos - tangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;  
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;

    // Attenuation
    float distance = length(tangentLightPos - tangentFragPos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * distance * distance);
    vec3 lighting = ambient + lightIntensity * attenuation * (diffuse + specular);
    FragColor = vec4(lighting, 1.0f);
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    float height =  texture(depthMap, texCoords).r;     
    return texCoords - viewDir.xy * (height * height_scale);        
}