#version 130

in vec2 uv;
in vec3 vert;

out vec4 fragColor;

uniform vec3 diffuse;

void main()
{
	fragColor = vec4(vert, 1);
	//fragColor = vec4(diffuse, 1.0);
}
