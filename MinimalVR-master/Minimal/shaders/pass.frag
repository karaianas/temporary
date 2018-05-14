#version 330 core

in vec2 UV;
in vec3 pos;
in vec3 normal;

uniform vec3 eyePos;

//layout(location = 0) out vec3 color;
out vec3 color;

uniform sampler2D renderedTexture;

void main()
{
    //color = vec3(1.0, 0.0, 0.0);
	vec3 temp = texture(renderedTexture, UV).xyz;
	vec3 view = normalize(eyePos - pos);
	float len = length(eyePos - pos);
	float dot = clamp(dot(view, normal), 0, 1);

	color = dot* temp;
	//color = texture( renderedTexture, UV).xyz;
}