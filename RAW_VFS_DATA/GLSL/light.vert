#version 120

//  directional & point light per pixel + multitexture

// передаем вертекс в фрагметный (уже нормализованную)
varying vec3 Normal;
// передаем вертекс в фрагметный
varying vec3 Vertex;

void main()
{
	Normal = normalize(gl_NormalMatrix * gl_Normal);
	Vertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	gl_Position = ftransform();
	gl_TexCoord[0]  = gl_MultiTexCoord0;
} 
