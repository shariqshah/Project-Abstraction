#version 130

out vec4 fragColor;

uniform vec3 diffuseColor;

void main()
{
	fragColor = vec4(diffuseColor, 1.0);
}
