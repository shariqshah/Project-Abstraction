#version 130

out vec4 fragColor;

in vec2 uv;
in vec3 color;

uniform vec4 diffuseColor;
uniform sampler2D sampler;

void main()
{
	vec4 texColor = texture2D(sampler, uv);
	fragColor     = diffuseColor * texColor;
}
