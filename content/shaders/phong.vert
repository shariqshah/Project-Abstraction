#version 130

in vec3 vPosition;
in vec3 vNormal;
in vec2 vUV;

out vec2 uv;
out vec3 normal;
out vec3 vertex;
// out vec3 vertexCamSpace;
// out vec4 vertexLightSpace;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 mvp;
// uniform mat4 projMat;
// uniform mat4 lightPVMat;

void main()
{
	gl_Position = mvp * vec4(vPosition, 1.0);
	uv = vUV;

	//TODO: account for scale by using inverse transpose matrix for normal
	//Normal and vertex sent to the fragment shader should be in the same space!
	normal = vec4(modelMat * vec4(vNormal, 0.0)).xyz;
	vertex = vec4(modelMat * vec4(vPosition, 1.0)).xyz;
	// vertexCamSpace = vec4(viewMat * vec4(vPosition, 1.0)).xyz;
	// vertexLightSpace = vec4((lightPVMat * modelMat) * vec4(vPosition, 1.0));
}
