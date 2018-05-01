#version 120

// directional & point light per pixel + geometry modification during explosion

// explosion's parameters
uniform float SpeedData1;
uniform float SpeedData2;

varying vec3 Normal; // already normalized
varying vec3 Vertex;


void main()
{
	// calculate current vertex location
	vec3 CenterPoint = vec3(gl_MultiTexCoord1.x, gl_MultiTexCoord1.y, gl_MultiTexCoord2.x);
	float Speed = gl_MultiTexCoord2.y;
	vec4 VertexData = gl_Vertex + vec4(CenterPoint * Speed * SpeedData2, 0.0);
	// move it to triangle's center
	VertexData = VertexData + vec4((CenterPoint - vec3(gl_Vertex)) * (1.0 - SpeedData1), 0.0);

	Vertex = vec3(gl_ModelViewMatrix * VertexData);
	Normal = normalize(gl_NormalMatrix * gl_Normal);

	gl_Position = gl_ModelViewProjectionMatrix * VertexData;
	gl_TexCoord[0]  = gl_MultiTexCoord0;
} 
