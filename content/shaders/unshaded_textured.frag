//include commonFrag.glsl common.glsl version.glsl

uniform sampler2D sampler;

void main()
{
	vec4 texColor = texture2D(sampler, uv);
	fragColor     = diffuseColor * texColor;
}
