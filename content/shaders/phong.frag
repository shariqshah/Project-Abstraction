#version 130

struct Fog
{
	int fogMode;
	float density;
	float start;
	float max;
	vec4 color;
};

struct Attenuation
{
	float constant;
	float linear;
	float quadratic;
};

struct Light
{
	vec3        color;
	vec3        direction;
	vec3        position;
	float       intensity;
	float       outerAngle;
	float       innerAngle;
	float       exponent;
	Attenuation attenuation;
	int         type;
};

struct Material
{
	float specular;
	float diffuse;
	float specularStrength;
};

in vec2 uv;
in vec3 normal;
in vec3 vertex;
// in vec3 vertexCamSpace;
// in vec4 vertexLightSpace;

out vec4 fragColor;

// uniform sampler2D texture;
// uniform sampler2D shadowMap;
uniform Fog fog;
uniform vec4 ambientLight;
uniform vec3 eyePos;
uniform vec4 diffuseColor;
uniform Material material;
uniform mat4 modelMat;

const int MAX_LIGHTS = 32;
const int FOG_NONE = 0;
const int FOG_LINEAR = 1;
const int FOG_EXPONENTIAL = 2;
const int FOG_EXPONENTIAL_SQRD = 3;
const int LT_SPOT  = 0;
const int LT_DIR   = 1;
const int LT_POINT = 2;

// uniform int numDirLights;
// uniform int numPointLights;
// uniform int numSpotLights;
uniform int numLights;

// uniform DirectionalLight dLights[MAX_LIGHTS];
// uniform PointLight pLights[MAX_LIGHTS];
// uniform SpotLight sLights[MAX_LIGHTS];
uniform Light lightList[MAX_LIGHTS];

// float calcShadowFactor(vec3 projCoords)
// {
// 	vec2 uvCoords;
// 	uvCoords.x = (projCoords.x * 0.5) + 0.5;
// 	uvCoords.y = (projCoords.y * 0.5) + 0.5;
// 	float z    = (projCoords.z * 0.5) + 0.5;

// 	//if uv outside shadowmap range then point out of shadow
// 	if(uvCoords.x > 1.0 || uvCoords.x < 0.0 ||
// 			uvCoords.y > 1.0 || uvCoords.y < 0.0)
// 	{
// 		return 1.0;
// 	}
// 	else
// 	{
// 		float depth = texture2D(shadowMap, uvCoords).r;

// 		if((depth + 0.009) < z)
// 		{
// 			return 0.3;
// 		}
// 		else
// 		{
// 			return 1.0;
// 		}
// 	}
// }

vec4 applyFog(vec4 color)
{
	vec4 finalColor = color;
	float fogFactor;
	float distFromEye = abs(length(vertex - eyePos));

	if(fog.fogMode != FOG_NONE)
	{
		if(fog.fogMode == FOG_LINEAR)
		{
			fogFactor = (fog.max - distFromEye) / (fog.max - fog.start);
		}
		else if(fog.fogMode == FOG_EXPONENTIAL)
		{
			fogFactor = exp(fog.density * -distFromEye);
		}
		else if(fog.fogMode == FOG_EXPONENTIAL_SQRD)
		{
			fogFactor = exp(-pow(fog.density * distFromEye, 2));
		}

		fogFactor = clamp(fogFactor, 0.0, 1.0);
		finalColor = mix(fog.color, color, fogFactor);
	}

	return finalColor;
}

vec4 calcDirLight(Light dLight)
{
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);

	vec3 normalizedNormal = normalize(normal);
	float cosAngIncidence = dot(normalizedNormal, -dLight.direction);
	cosAngIncidence = clamp(cosAngIncidence, 0, 1);
	// float shadowFactor = calcShadowFactor(vertexLightSpace.xyz);

	if(cosAngIncidence > 0)
	{
		diffuse = dLight.color * material.diffuse * cosAngIncidence;

		vec3 vertexToEye = normalize(eyePos - vertex);
		vec3 lightReflect = normalize(reflect(dLight.direction, normalizedNormal));
		float specularFactor = max(0.0, dot(vertexToEye, lightReflect));
		specularFactor = pow(specularFactor, material.specularStrength);
		specular = dLight.color * material.specular * specularFactor;
	}

	// return dLight.intensity * shadowFactor * (diffuse + specular);
	return vec4(dLight.intensity * (diffuse + specular), 1.0);
}

vec4 calcPointLight(Light light)
{
	vec3 diffuse  = vec3(0.0);
	vec3 specular = vec3(0.0);

	vec3 lightDirection = vertex - light.position;
	float distance = length(lightDirection);

	if(distance < 100) //TODO: Find some other method rather than hard coding
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

		float attenuation = (light.attenuation.constant) +
							(light.attenuation.linear * distance) +
							(light.attenuation.quadratic * distance * distance);

		return vec4(((diffuse + specular) / attenuation) * light.intensity, 1.0);
	}
	else
		return vec4(0.0);

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

void main()
{
	// vec4 pixelColor = texture2D(texture, uv.st);
	vec4 pixelColor = diffuseColor;
	
	// vec4 totalDlightsColor = vec4(0.0);
	// vec4 totalPlightsColor = vec4(0.0);
	// vec4 totalSlightsColor = vec4(0.0);
	vec4 totalLightColor   = vec4(0.0);

	for(int i = 0; i < numLights; i++)
	{
		switch(lightList[i].type)
		{
		case LT_DIR:
			totalLightColor += calcDirLight(lightList[i]);
			break;
		case LT_SPOT:
			totalLightColor += calcSpotLight(lightList[i]);
			break;
		case LT_POINT:
			totalLightColor += calcPointLight(lightList[i]);
			break;
		}
	}
	// //Directional Lights
	// if(numDirLights > 0)
	// {
	// 	for(int i = 0; i < numDirLights; i++)
	// 	{
	// 		totalDlightsColor += calcDirLight(dLights[i]);
	// 	}
	// }

	// //Point Lights
	// if(numPointLights > 0)
	// {
	// 	for(int i = 0; i < numPointLights; i++)
	// 	{
	// 		totalPlightsColor += calcPointLight(pLights[i]);
	// 	}
	// }

	// //Spot Lights
	// if(numSpotLights > 0)
	// {
	// 	for(int i = 0; i < numSpotLights; i++)
	// 	{
	// 		totalSlightsColor += calcSpotLight(sLights[i]);
	// 	}
	// }

	// totalLightColor = totalDlightsColor + totalPlightsColor + totalSlightsColor;
	vec4 finalColor = applyFog(totalLightColor);

	fragColor = (pixelColor * (ambientLight + finalColor));
}
