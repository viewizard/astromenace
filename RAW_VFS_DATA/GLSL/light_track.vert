#version 110

//  directional & point light per pixel + multitexture

// глобальные данные тайловой анимации траков
uniform float TrackSpeed;

// передаем вертекс в фрагметный (уже нормализованную)
varying vec3 Normal;
// передаем вертекс в фрагметный
varying vec3 Vertex;

void main()
{
	Normal = normalize(gl_NormalMatrix * gl_Normal);
	Vertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	gl_Position = ftransform();
	gl_TexCoord[0].x  = gl_MultiTexCoord0.x + TrackSpeed;
	gl_TexCoord[0].y  = gl_MultiTexCoord0.y;
} 
