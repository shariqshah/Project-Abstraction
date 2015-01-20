//include fog.glsl phongCommon.glsl common.glsl commonFrag.glsl version.glsl 

void main()
{
	// vec4 pixelColor = texture2D(texture, uv.st);
	vec4 pixelColor = diffuseColor;
	vec4 totalLightColor = doForwardLightLoop();
	vec4 finalColor = applyFog(totalLightColor);
	fragColor = (pixelColor * (ambientLight + finalColor));
	
}
