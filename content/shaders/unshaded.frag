//include fog.glsl common.glsl commonFrag.glsl version.glsl

void main()
{
	fragColor = applyFog(diffuseColor);
}
