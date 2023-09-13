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
uniform float minLayers;
uniform float maxLayers;

layout (std140) uniform LightProperties {
    float lightIntensity;
    float constant;
    float linear;
    float quadratic;
};

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);
vec2 SteepParallaxMapping(vec2 texCoords, vec3 viewDir);

void main()
{
	vec3 viewDir = normalize(tangentViewPos - tangentFragPos);

    // Retrieving texCoords from depth(height) map
	vec2 texCoords = SteepParallaxMapping(TexCoords, viewDir);

    // Notice: this can only work on a squad
    if (texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0) 
        discard;

	// normal
    vec3 normal = texture(normalMap, texCoords).rgb;
	normal = normalize(normal * 2.0f - 1.0f);

	// Calculate ambient, diffuse and specular coefficients
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
    return texCoords - viewDir.xy * (height * height_scale); // use minus here since we use depth map(not height map)        
}

// Apply multiple samples to improve accuracy
vec2 SteepParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    // Dynamic layer adjustment using uniform variables
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
    float deltaLayerDepth = 1.0f / numLayers;

    vec2 deltaTexCoords = viewDir.xy / (viewDir.z * numLayers);
    vec2 currentTexCoords = texCoords;
    float currentLayerDepth = 0.0;
    float currentHeight = 0.0f;

    // Ray marching loop
    for (int i = 0; i < numLayers; ++i) {
        currentHeight = texture(depthMap, currentTexCoords).r * height_scale;
        if (currentHeight < currentLayerDepth) {
            // We found the intersection point
            break;
        }
        currentTexCoords -= deltaTexCoords;
        currentLayerDepth += 1.0 / numLayers;
    }
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    float afterDepth = currentLayerDepth - currentHeight;
    float beforeDepth = currentLayerDepth - deltaLayerDepth - texture(depthMap, prevTexCoords).r * height_scale;

    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}