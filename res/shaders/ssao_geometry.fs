#version 330 core
layout(location = 0) out vec4 gPositionDepth;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform float plane_near; // Projection matrix's near plane distance
uniform float plane_far; // Projection matrix's far plane distance

// Linearizes a depth value stored in the depth buffer.
// The function takes a depth value in the range [0, 1] and converts it to a linear depth value.
// This is useful for certain lighting calculations and depth comparisons.
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * plane_near * plane_far) / (plane_far + plane_near - z * (plane_far - plane_near));	
}

void main()
{
    // Store the fragment position vector in the first gbuffer texture
    gPositionDepth.xyz = FragPos;
    // And store linear depth into gPositionDepth's alpha component
    gPositionDepth.a = LinearizeDepth(gl_FragCoord.z); // Divide by FAR if you need to store depth in range 0.0 - 1.0 (if not using floating point colorbuffer)
    // Also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // And the diffuse per-fragment color
    gAlbedoSpec.rgb = vec3(0.95); // Currently all objects have constant albedo color
}