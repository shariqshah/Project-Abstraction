#version 130

out vec4 fragColor;

in vec2 uv;
in vec3 color;

uniform vec3 diffuseColor;
uniform sampler2D sampler;
void main()
{
	// fragColor = vec4(diffuseColor, 1.0);
	fragColor = texture2D(sampler, uv);
	// fragColor = vec4(color, 1);
}
