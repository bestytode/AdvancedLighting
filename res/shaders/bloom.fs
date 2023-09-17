#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

void main()
{

	// first do normal lighting calculations and output results
	// todo
	FragColor = vec4(lighting, 1.0f);

	// Check whether fragment output is higher than threshold, if so output as brightness color
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0f)
	    BrightColor = vec4(FragColor.rgb, 1.0f);
}