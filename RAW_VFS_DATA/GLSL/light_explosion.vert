#version 120

//  directional & point light per pixel + explosion geometry modification

// глобальные данные взрыва
uniform float SpeedData1;
uniform float SpeedData2;

// передаем нормаль в фрагметный (уже нормализованную)
varying vec3 Normal;
// передаем вертекс в фрагметный
varying vec3 Vertex;

void main()
{
	// находим текущее положение вертекса
	vec3 CenterPoint = vec3(gl_MultiTexCoord1.x, gl_MultiTexCoord1.y, gl_MultiTexCoord2.x);
	// скорость
	float Speed = gl_MultiTexCoord2.y;

	// находим положение точки
	vec4 VertexData = gl_Vertex + vec4(CenterPoint*Speed*SpeedData2, 0.0);
	// а теперь сдвигаем ее к центру треугольника
	VertexData = VertexData + vec4((CenterPoint-vec3(gl_Vertex))*(1.0-SpeedData1), 0.0);

	Vertex = vec3(gl_ModelViewMatrix * VertexData);
	Normal = normalize(gl_NormalMatrix * gl_Normal);

	gl_Position = gl_ModelViewProjectionMatrix * VertexData;
	gl_TexCoord[0]  = gl_MultiTexCoord0;
} 
