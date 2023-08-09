#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform sampler2D diffuseTexture;
uniform sampler2D depthMap;

uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform vec3 viewPos;

bool ShadowCalculation(vec4 fragPosLightSpace);

void main()
{
	vec3 color = texture(diffuseTexture, TexCoords).rgb;
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(1.0);

    // Ambient
    vec3 ambient = 0.15 * color;

    // Diffuse
    vec3 lightDir = normalize(-lightDirection);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;   
    
    // shadow
    vec3 lighting;
    bool isInShadow = ShadowCalculation(FragPosLightSpace);

    if (isInShadow) {
        lighting = ambient * color;
    }
    else {
        lighting = (ambient + diffuse + specular) * color;
    } 

    FragColor = vec4(lighting, 1.0f);
}

bool ShadowCalculation(vec4 fragPosLightSpace)
{
    // Calculations
    fragPosLightSpace /= fragPosLightSpace.w;
    vec3 projCoords = fragPosLightSpace.xyz * 0.5 + 0.5;

    // Retrieve the stored depth and current depth
    float closestDepth = texture(depthMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    // Since depth range is from 0 to 1, the greater depth value is, 
    // actually the further the object is becomming.
    if (currentDepth > closestDepth) 
        return true;

    return false;
}