#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    

	//vec3 temp =  TexCoords * vec3(-1, 1, 1);
	FragColor = texture(skybox, TexCoords);
}

/*
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec4 oColor;

uniform sampler2D texture_diffuse1;

void main()
{    
    //FragColor = texture(texture_diffuse1, TexCoords);
	//FragColor = vec4(1, 0, 0, 1);
	FragColor = oColor;
}
*/