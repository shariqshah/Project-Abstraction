//include version.glsl

in vec2 uv;
in vec4 color;

out vec4 fragColor;

uniform sampler2D sampler;

void main()
{
	fragColor = color * texture(sampler, uv);
}
