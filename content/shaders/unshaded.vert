#version 130

in vec3 vPosition;
in vec2 vUV;

out vec2 uv;
out vec3 vert;

uniform mat4 mvp;
uniform mat4 viewMat;
uniform mat4 projMat;

void main()
{
	uv = vUV;
	vert = vPosition;
	gl_Position = mvp * vec4(vPosition, 1.0);
	//gl_Position = vec4(vPosition, 1.0);
	//gl_Position = viewMat * vec4(vPosition, 1.0);
}
