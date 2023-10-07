#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D noiseTexture;

uniform vec3 samples[64];
uniform float kernerlSize;
uniform float radius;

uniform mat4 projection;

// tile noise texture over screen based on screen dimensions divided by noise size
const vec2 noiseScale = vec2(1920.0f / 4.0f, 1080.0f / 4.0f); 

void main()
{
    // Retrieve Position and Normal from G-Buffer, noise vector from noiseTexture
    vec3 FragPos = texture(gPositionDepth, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 randomVec = texture(noiseTexture, TexCoords * noiseScale).rgb;

    // Create TBN change-of-basis matrix: from tangent-space to view-space
    // The TBN matrix is used to transform the sample points into an orientation
    // that aligns with the fragment's normal. This is essential for correct SSAO.
    // The TBN matrix also incorporates randomness through randomVec to make
    // the SSAO look more natural and avoid visual artifacts.
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // SSAO Kernel Loop
    float occlusion = 0.0f;
    for(int i = 0; i < kernelSize; i++) {
        // Transform the sample point from tangent space to view space.
        // The sample point is fetched from a pre-computed array of points that
        // are oriented along the z-axis in tangent space.
        vec3 sample = TBN * samples[i]; // tangent space -> view space
        sample = FragPos + sample * radius;

        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(sample, 1.0f);
        offset = projection * offset; // view space -> clip space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5f + 0.5; // transform to range 0.0 - 1.0

        // Retrieve the linearized depth value from the gPositionDepth texture.
        // Negate the value because the camera looks down the negative Z-axis in view space.
        float sampleDepth = -texture(gPositionDepth, offset.xy).w; 

        // Introduce a rangeCheck to ensure that it only affect the occlusion factor when the measured depth value is within the radius
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(FragPos.z - sampleDepth));

        // Compare the actual depth of the scene at the sample point (sampleDepth)
        // with the depth of the sample point itself (sample.z).
        // If sampleDepth is greater, it means the sample point is not occluded.
        occlusion += (sampleDepth >= sample.z ? 1.0 : 0.0) * rangeCheck;   
    }

    // Normalization & inversion for later use
    // serves as a coefficient can use to modulate the lighting
    occlusion = 1.0 - (occlusion / kernelSize);
    FragColor = occlusion;
}



// Initialize Variables
// -Initialize a variable to accumulate the occlusion factor. Let's call it occlusion.

// SSAO Kernel Loop
// -Loop through the SSAO kernel samples. For each sample:
// -Transform Sample
// --Rotate the sample point in the hemisphere to be oriented around the normal at the fragment.
// -Sample Position
// --Add the rotated and scaled sample to the fragment's position.
// -Depth Comparison
// --Compare the depth of the sampled position with the depth at the fragment to determine if the sample point is occluded.
// -Accumulate Occlusion
// --If the sample point is occluded, increment the occlusion variable.

// Normalize Occlusion
// -Divide the occlusion by the number of samples (kernelSize) to get an average.

// sOutput Occlusion
// -Output the occlusion as the fragment color.