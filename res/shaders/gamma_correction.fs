#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D floorTexture;
uniform bool gamma;
uniform vec3 viewPos;
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

vec3 BlinnPhong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor);

void main()
{
    vec3 color = texture(floorTexture, TexCoords).rgb;
    vec3 lighting = vec3(0.0f);
    for(int i = 0; i < 4; i++) {
        lighting = BlinnPhong(Normal, FragPos, lightPositions[i], lightColors[i]);
        color += lighting;
    }

    if(gamma) {
        color = pow(color, vec3(1.0 / 2.2));
    }
    FragColor = vec4(color, 1.0f);
}

vec3 BlinnPhong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor)
{
    // Properties
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);

    // Ambient
    // ignore here

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

    // Specular
    vec3 halfwayDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(halfwayDir, norm), 0.0f) , 64.0f);
    vec3 specular = spec * lightColor;

    // Simple attenuation
    float attenuation = 0.0f;
    float distance = length(lightPos - fragPos);

    // Attenuation
    //if(gamma) 
        attenuation = 1.0 / (distance * distance);
    //else 
        //attenuation = 1.0 / distance;
    
    //attenuation = 1.0 / (1 + 0.09 * distance + 0.032 * distance * distance);
    diffuse *= attenuation;
    specular *= attenuation;

    return diffuse + specular;
}