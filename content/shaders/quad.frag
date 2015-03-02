#version 130

in vec2 uv;

out vec4 fragColor;

uniform vec4 textColor;
uniform sampler2D sampler;

void main()
{
	// fragColor = vec4(1, 1, 1, 1) * vec4(textColor, 1);
	// fragColor = vec4(1, 1, 1, 1) * texture2D(sampler, uv);
	fragColor = texture2D(sampler, uv) * textColor;
	// fragColor = textColor;
	// fragColor = vec4(uv, 0, 1.0);
	// fragColor = texture2D(sampler, uv);
}
