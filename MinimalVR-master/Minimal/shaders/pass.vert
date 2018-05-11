#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 UV;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
	UV = (aPos.xy + vec2(1,1))/2.0;
}