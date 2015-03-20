//include fog.glsl phongCommon.glsl common.glsl commonFrag.glsl version.glsl 

void main()
{
	vec4 totalLightColor = doForwardLightLoop();
	fragColor            = applyFog(diffuseColor * (totalLightColor + ambientLight));
}
