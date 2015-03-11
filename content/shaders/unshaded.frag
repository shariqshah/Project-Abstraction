//include fog.glsl commonFrag.glsl common.glsl version.glsl

void main()
{
	fragColor = (ambientLight * diffuseColor) + applyFog(diffuseColor);
}
