#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec2 TexCoords;
out vec3 tangentLightPos;
out vec3 tangentViewPos;
out vec3 tangentFragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    vec3 FragPos = vec3(model * vec4(position, 1.0f));
    TexCoords = texCoords;
    
    // Compute the normal matrix from the model matrix
    mat3 normalMatrix = transpose(inverse(mat3(model)));

    // Transform tangent, bitangent, and normal vectors to world space
    vec3 T = normalize(normalMatrix * tangent);
    vec3 B = normalize(normalMatrix * bitangent);
    vec3 N = normalize(normalMatrix * normal);   

    // Construct the TBN matrix. 
    // *** IMPORTANT: Transpose is optional and depends on your specific needs.
    // It's used here to transform lightPos, viewPos, and FragPos to tangent space.
    mat3 TBN = transpose(mat3(T, B, N));  

    // Transform lightPos, viewPos, and FragPos from world space to tangent space
    tangentLightPos = TBN * lightPos;
    tangentViewPos = TBN * viewPos;
    tangentFragPos = TBN * FragPos;
}