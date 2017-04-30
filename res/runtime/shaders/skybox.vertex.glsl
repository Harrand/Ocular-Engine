#version 430

layout(location = 0) in vec3 position;
out vec3 vs_cubePosition_modelspace;

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

void main()
{
	gl_Position = (p * v * m) * vec4(position, 1.0);
	vs_cubePosition_modelspace = position;
}