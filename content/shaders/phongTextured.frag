//include fog.glsl phongCommon.glsl common.glsl commonFrag.glsl version.glsl 

uniform sampler2D sampler;

void main()
{
	vec4 pixelColor = diffuseColor * texture2D(sampler, uv);
	vec4 totalLightColor = doForwardLightLoop();
	vec4 finalColor = applyFog(totalLightColor);
	// fragColor = pixelColor * (ambientLight + finalColor);
	fragColor = (pixelColor * ambientLight) + (pixelColor * finalColor);
}
