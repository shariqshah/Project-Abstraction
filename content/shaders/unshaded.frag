//include fog.glsl commonFrag.glsl common.glsl version.glsl

void main()
{
	fragColor = ambientLight + applyFog(diffuseColor);
}
