//include fog.glsl commonFrag.glsl common.glsl version.glsl

uniform sampler2D sampler;

void main()
{
	vec4 pixelColor = diffuseColor * texture2D(sampler, uv);
	fragColor       = (ambientLight * pixelColor) + applyFog(pixelColor);
}
