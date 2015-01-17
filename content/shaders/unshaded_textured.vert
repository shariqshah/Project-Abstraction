#version 130

in vec3 vPosition;
in vec2 vUV;
in vec3 vColor;

out vec2 uv;
out vec3 color;
uniform mat4 mvp;

void main()
{
	uv = vUV;
	color = vColor;
	gl_Position = mvp * vec4(vPosition, 1.0);
}
