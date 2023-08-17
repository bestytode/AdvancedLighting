#version 330 core
in vec4 FragPos;

uniform vec3 LightPos;
uniform float far_plane;

void main()
{
	// Calculate distance between fragment and light source and map to [0;1] 
	float lightDistance = length(FragPos.xyz - LightPos) / far_plane;

	// Write this as modified depth
	gl_FragDepth = lightDistance;
}