#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform sampler2D diffuseTexture;
uniform vec3 viewPos;

void main()
{           
    vec3 color = texture(diffuseTexture, TexCoords).rgb;
    vec3 normal = normalize(Normal);

    // ambient
    vec3 ambient = 0.0 * color;
    // lighting
    vec3 lighting = vec3(0.0);
    vec3 viewDir = normalize(viewPos - FragPos);

    for (int i = 0; i < 4; i++) {
        // diffuse
        vec3 lightDir = normalize(lightPositions[i] - FragPos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 result = lightColors[i] * diff * color;      

        // attenuation (use quadratic as we have gamma correction)
        float distance = length(FragPos - lightPositions[i]);
        result *= 1.0 / (distance * distance);
        lighting += result;
    }
    vec3 result = ambient + lighting;

    // check whether result is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(result, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    FragColor = vec4(result, 1.0);
}