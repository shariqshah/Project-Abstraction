
struct Light
{
	vec4  color;
	vec3  direction;
	vec3  position;
	float intensity;
	float outerAngle;
	float innerAngle;
	float falloff;
	int   radius;
	int   type;
	int   castShadow;
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
uniform Light light;
uniform sampler2D shadowMap;

float calcShadowFactor(vec3 projCoords)
{
	float bias = 0.5;
	vec2 uvCoords;
	uvCoords.x = (projCoords.x * bias) + bias;
	uvCoords.y = (projCoords.y * bias) + bias;
	float z    = (projCoords.z * bias) + bias;

	//if uv outside shadowmap range then point out of shadow
	if(uvCoords.x > 1.0 || uvCoords.x < 0.0 ||	uvCoords.y > 1.0 || uvCoords.y < 0.0)
	{
		return 1.0;
	}
	else
	{
		float depth = texture2D(shadowMap, uvCoords).r;
		if((depth + 0.001) < z)
			return 0.1;
		else
			return 1.0;
	}
}

vec4 calcDirLight(Light dirLight)
{
	vec4  diffuse  = vec4(0.0);
	vec4  specular = vec4(0.0);
	vec3  normalizedNormal = normalize(normal);
	float cosAngIncidence  = dot(normalizedNormal, -dirLight.direction);
	cosAngIncidence = clamp(cosAngIncidence, 0, 1);
	float shadowFactor = 1.0;
	
	if(cosAngIncidence > 0)
	{
		diffuse = dirLight.color * material.diffuse * cosAngIncidence;

		vec3  vertexToEye    = normalize(eyePos - vertex);
		vec3  lightReflect   = normalize(reflect(dirLight.direction, normalizedNormal));
		float specularFactor = max(0.0, dot(vertexToEye, lightReflect));
		specularFactor = pow(specularFactor, material.specularStrength);
		specular = dirLight.color * material.specular * specularFactor;
		if(light.castShadow == 1)
		{
			shadowFactor = calcShadowFactor(vertLightSpace.xyz);
		}
	}
	// return dirLight.intensity * shadowFactor * (diffuse + specular);
	return (dirLight.intensity * (diffuse + specular)) * shadowFactor;
}

vec4 calcPointLight(Light pointLight)
{
	vec4  diffuse  = vec4(0.0);
	vec4  specular = vec4(0.0);
	vec3  lightDirection = vertex - pointLight.position;
	float distance = abs(length(lightDirection));

	if(distance <= pointLight.radius)
	{
		lightDirection = normalize(lightDirection);
		vec3 normalizedNormal = normalize(normal);
		float cosAngIncidence = dot(normalizedNormal, -lightDirection);
		cosAngIncidence = clamp(cosAngIncidence, 0, 1);

		if(cosAngIncidence > 0)
		{
			diffuse = pointLight.color * material.diffuse * cosAngIncidence;
			vec3 vertexToEye = normalize(eyePos - vertex);
			vec3 lightReflect = normalize(reflect(lightDirection, normalizedNormal));
			float specularFactor = max(0.0, dot(vertexToEye, lightReflect));
			specularFactor = pow(specularFactor, material.specularStrength);
			specular = pointLight.color * material.specular * specularFactor;
		}
		float attenuation = pow(max(0.0, (1.0 - (distance / pointLight.radius))), pointLight.falloff + 1.0f);
		return (((diffuse + specular) * attenuation) * pointLight.intensity);
	}
	else
	{
		return vec4(0.0);
	}
}

vec4 calcSpotLight(Light spotLight)
{
	vec4 color = vec4(0.0);
	vec3 lightToSurface = vertex - spotLight.position;
	float angle = dot(spotLight.direction, normalize(lightToSurface));
	if(acos(angle) < spotLight.outerAngle)
	{
		color = calcPointLight(spotLight);
		color *= smoothstep(cos(spotLight.outerAngle), cos(spotLight.innerAngle), angle);
		if(light.castShadow != 0)
		{
			float shadowFactor = calcShadowFactor(vertLightSpace.xyz / vertLightSpace.w);
			color *= shadowFactor;
		}
	}
	return color;// * shadowFactor;
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

vec4 calculateLight()
{
	vec4 totalLightColor = vec4(0.0);
	switch(light.type)
	{
	case LT_DIR:   totalLightColor = calcDirLight(light);   break;
	case LT_SPOT:  totalLightColor = calcSpotLight(light);  break;
	case LT_POINT: totalLightColor = calcPointLight(light); break;
	}
	return totalLightColor;
}
