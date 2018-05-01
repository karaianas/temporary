#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
	vec3 temp =  TexCoords * vec3(-1, 1, 1);
	FragColor = texture(skybox, temp);
}
