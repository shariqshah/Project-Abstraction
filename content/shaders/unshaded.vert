#version 130

in vec3 vPosition;
in vec2 vUV;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(vPosition, 1.0);
}
