#version 120

//  directional & point light per pixel + normal mapping

// передаем вертекс в фрагметный (уже нормализованную)
varying vec3 pNormal;
varying vec3 pTangent;
varying vec3 pBinormal;
uniform int NeedNormalMapping;

// передаем вертекс в фрагметный
varying vec3 Vertex;

void main()
{
	pNormal = normalize(gl_NormalMatrix * gl_Normal);
	// находим тангент и бинормаль
	if (NeedNormalMapping == 1)
	{
		vec3 vTangent = vec3(gl_MultiTexCoord1.st, gl_MultiTexCoord2.s);
		pTangent  = normalize(gl_NormalMatrix * vTangent);
		pBinormal = normalize(gl_NormalMatrix * (cross(gl_Normal, vTangent) * gl_MultiTexCoord2.t));
	}

	Vertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0]  = gl_TextureMatrix[0] * gl_MultiTexCoord0;
} 
