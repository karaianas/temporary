#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 eyePos;
uniform vec3 planeN;

out vec2 UV;
out vec3 pos;
out vec3 normal;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
	pos = vec3( model * vec4(aPos, 1.0));
	normal = planeN;
	UV = (aPos.xy + vec2(1,1))/2.0;
}