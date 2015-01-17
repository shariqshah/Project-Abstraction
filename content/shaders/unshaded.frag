#version 130

out vec4 fragColor;

in vec2 uv;
in vec3 color;

uniform vec4 diffuseColor;

void main()
{
	fragColor = diffuseColor;
}
