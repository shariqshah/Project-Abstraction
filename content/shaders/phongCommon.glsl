
struct Light
{
	vec4  color;
	vec3  direction;
	vec3  position;
	float intensity;
	float outerAngle;
	float innerAngle;
	float exponent;
	float falloff;
	int   radius;
	int   type;
};

struct Material
{
	float specular;
	float diffuse;
	float specularStrength;
};

const int MAX_LIGHTS = 32;
const int LT_SPOT    = 0;
const int LT_DIR     = 1;
const int LT_POINT   = 2;

uniform Material material;
uniform int numLights;
uniform Light lightList[MAX_LIGHTS];

vec4 calcDirLight(Light dLight)
{
	vec4  diffuse  = vec4(0.0);
	vec4  specular = vec4(0.0);
	vec3  normalizedNormal = normalize(normal);
	float cosAngIncidence  = dot(normalizedNormal, -dLight.direction);
	cosAngIncidence = clamp(cosAngIncidence, 0, 1);
	// float shadowFactor = calcShadowFactor(vertexLightSpace.xyz);

	if(cosAngIncidence > 0)
	{
		diffuse = dLight.color * material.diffuse * cosAngIncidence;

		vec3  vertexToEye    = normalize(eyePos - vertex);
		vec3  lightReflect   = normalize(reflect(dLight.direction, normalizedNormal));
		float specularFactor = max(0.0, dot(vertexToEye, lightReflect));
		specularFactor = pow(specularFactor, material.specularStrength);
		specular = dLight.color * material.specular * specularFactor;
	}
	// return dLight.intensity * shadowFactor * (diffuse + specular);
	return (dLight.intensity * (diffuse + specular));
}

vec4 calcPointLight(Light light)
{
	vec4  diffuse  = vec4(0.0);
	vec4  specular = vec4(0.0);
	vec3  lightDirection = vertex - light.position;
	float distance = abs(length(lightDirection));

	if(distance <= light.radius)
	{
		lightDirection = normalize(lightDirection);
		vec3 normalizedNormal = normalize(normal);
		float cosAngIncidence = dot(normalizedNormal, -lightDirection);
		cosAngIncidence = clamp(cosAngIncidence, 0, 1);

		if(cosAngIncidence > 0)
		{
			diffuse = light.color * material.diffuse * cosAngIncidence;
			vec3 vertexToEye = normalize(eyePos - vertex);
			vec3 lightReflect = normalize(reflect(lightDirection, normalizedNormal));
			float specularFactor = max(0.0, dot(vertexToEye, lightReflect));
			specularFactor = pow(specularFactor, material.specularStrength);
			specular = light.color * material.specular * specularFactor;
		}
		float attenuation = pow(max(0.0, (1.0 - (distance / light.radius))), light.falloff + 1.0f);
		return (((diffuse + specular) * attenuation) * light.intensity);
	}
	else
	{
		return vec4(0.0);
	}
}

vec4 calcSpotLight(Light light)
{
	vec4 color = vec4(0.0);
	vec3 lightToSurface = vertex - light.position;
	float angle = dot(light.direction, normalize(lightToSurface));
	// float shadowFactor = calcShadowFactor(vertexLightSpace.xyz / vertexLightSpace.w);
	if(acos(angle) < light.outerAngle)
	{
		color = calcPointLight(light);
		color *= smoothstep(cos(light.outerAngle), cos(light.innerAngle), angle);
		// color *= shadowFactor;
	}
	return color; //* shadowFactor;
}

vec4 doForwardLightLoop()
{
	vec4 totalLightColor = vec4(0.0);
	for(int i = 0; i < numLights; i++)
	{
		switch(lightList[i].type)
		{
		case LT_DIR:   totalLightColor += calcDirLight(lightList[i]);   break;
		case LT_SPOT:  totalLightColor += calcSpotLight(lightList[i]);  break;
		case LT_POINT: totalLightColor += calcPointLight(lightList[i]); break;
		}
	}
	return totalLightColor;
}
