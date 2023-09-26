#version 330 core
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec2 gAlbedoSpecular;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
	gPosition = FragPos;
	gNormal = normalize(Normal);
	gAlbedoSpecular.rgb = texture(texture_diffuse1, TexCoords).rgb;
	gAlbedoSpecular.a = texture(texture_specular1, TexCoords).r;
}