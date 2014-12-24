[[FX]]

samplerCube buf0 = sampler_state
{
	Address = Clamp;
	// Filter = Bilinear;
	// MaxAnisotropy = 1;
};
	
context FINALPASS
{
	VertexShader = compile GLSL VS_FSQUAD;
	PixelShader = compile GLSL FS_FINALPASS;
	
	ZWriteEnable = false;
}
	
[[VS_FSQUAD]]
// =================================================================================================

#include "shaders/utilityLib/vertCommon.glsl"

uniform mat4 projMat;
uniform mat4 viewProjMat;
attribute vec3 vertPos;
attribute vec2 texCoords0;
attribute vec2 texCoords1;
varying vec2 texCoords;

void main( void )
{
    //texCoords = vec2( texCoords0.s, -texCoords0.t );
	texCoords = texCoords0;
	// texCoords   = gl_MultiTexCoord0.st;
	// texCoords = vec2(texCoords0.s, -texCoords0.t);
	//texCoords = texCoords1;
    // gl_Position = viewProjMat * calcWorldPos(vec4(vertPos, 1.0));
	// gl_Position = gl_ProjectionMatrix * gl_Vertex;
	gl_Position = projMat * vec4(vertPos, 1.0);
}


// attribute vec2 texCoords0;
// attribute vec3 vertPos;
// varying vec2 texCoords;
				
// void main( void )
// {
// 	//texCoords   = gl_MultiTexCoord0.st;
// 	texCoords = vec2(texCoords0.s, -texCoords0.t);
// 	gl_Position = gl_ProjectionMatrix * gl_Vertex;
// }

[[FS_FINALPASS]]
// =================================================================================================

uniform sampler2D buf0;
uniform vec2 frameBufSize;
varying vec2 texCoords;

void main( void )
{	
	// just copy it
	gl_FragColor = texture2D(buf0, texCoords);
	// gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	// gl_FragColor = texture2D(buf0, vec2(0.5, 0.5));
	// gl_FragColor = vec4(texCoords, 0.0, 1.0);
}
