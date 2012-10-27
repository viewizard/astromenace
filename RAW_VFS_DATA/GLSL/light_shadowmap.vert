#version 120

//  directional & point light per pixel  + shadow map with PCF + multitexture

// передаем вертекс в фрагметный (уже нормализованную)
varying vec3 Normal;
// передаем вертекс в фрагметный
varying vec3 Vertex;

void main()
{
	Normal = normalize(gl_NormalMatrix * gl_Normal);
	Vertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

	// делаем установку из текстурной матрицы шадовмепа
	gl_TexCoord[2] = gl_TextureMatrix[2] * gl_ModelViewMatrix * gl_Vertex;
} 
