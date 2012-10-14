#version 120

//  directional & point light per pixel + multitexture

// матрица вращения ствола
uniform mat4 TurrenRotationMat4;

// передаем вертекс в фрагметный (уже нормализованную)
varying vec3 Normal;
// передаем вертекс в фрагметный
varying vec3 Vertex;

void main()
{
	Normal = normalize(gl_NormalMatrix * vec3(TurrenRotationMat4 * vec4(gl_Normal, 0.0)));
	Vertex = vec3(gl_ModelViewMatrix * TurrenRotationMat4 * gl_Vertex);

	gl_Position = gl_ModelViewProjectionMatrix * TurrenRotationMat4 * gl_Vertex;
	gl_TexCoord[0]  = gl_MultiTexCoord0;
} 
