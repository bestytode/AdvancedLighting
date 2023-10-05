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
const vec2 noiseScale = vec2(1920.0f/4.0f, 1080.0f/4.0f); 

void main()
{

}

// Retrieve Position and Normal from G-Buffer
// -Sample the gPositionDepth and gNormal textures to get the fragment's position and normal in world space.

// Retrieve Noise Vector
// -Sample the noiseTexture using TexCoords scaled by noiseScale to get a random rotation vector.

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