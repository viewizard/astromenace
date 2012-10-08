#version 110

// Вертексный шейдер для системы частиц

// передаем на фрагментный шейдер цвет вертекса
varying vec4 VertexColor;
// получаем из основной программы положение камеры
uniform vec3 CameraPoint;


void main(void)
{ 
	// Размер частицы, передаем его как координату текстуры
	float ParticleSize = gl_MultiTexCoord0[1];

	// находим вектор камера-точка
	vec3 nnTmp = normalize(CameraPoint - gl_Vertex.xyz);

	// номер вертекса в квадрате, передаем как параметр текстуры
	float VertexNum = gl_MultiTexCoord0[0];
	vec4 Vertex;

	// находим перпендикуляр к вектору nnTmp
	vec3 nnTmp2 = normalize(vec3(1.0, 1.0, -(nnTmp[0] + nnTmp[1])/nnTmp[2]));

	// согласно номеру вертекса, ставим правильные координаты текстуры и корректируем положение
	if (VertexNum == 1.0 || VertexNum == 3.0)
	{
		// находим перпендикуляр к векторам nnTmp и nnTmp2
		vec3 nnTmp3 = cross(nnTmp,nnTmp2);
		nnTmp3 = normalize(nnTmp3);

		if (VertexNum == 1.0)
		{
			Vertex = gl_Vertex+vec4(nnTmp3*(-ParticleSize*1.5), 0.0);
			gl_TexCoord[0][0] = 0.0;
			gl_TexCoord[0][1] = 1.0;
		}
		else // 3.0
		{
			Vertex = gl_Vertex+vec4(nnTmp3*(ParticleSize*1.5), 0.0);
			gl_TexCoord[0][0] = 1.0;
			gl_TexCoord[0][1] = 0.0;
		}
	}
	else
	{
		if (VertexNum == 2.0)
		{
			Vertex = gl_Vertex+vec4(nnTmp2*(ParticleSize*1.5), 0.0);
			gl_TexCoord[0][0] = 0.0;
			gl_TexCoord[0][1] = 0.0;
		}
		else // 4.0
		{
			Vertex = gl_Vertex+vec4(nnTmp2*(-ParticleSize*1.5), 0.0);
			gl_TexCoord[0][0] = 1.0;
			gl_TexCoord[0][1] = 1.0;
		}
	}

	// передаем цвет частицы
	VertexColor = gl_Color;
	// передаем положение вертекса
	gl_Position = gl_ModelViewProjectionMatrix * Vertex;
}
