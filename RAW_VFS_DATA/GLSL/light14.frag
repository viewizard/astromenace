#version 110

//  directional & point light per pixel + multitexture

// кол-во используемых источников (используем с 1 по N, вот это N сюда и передаем, не знаю как узнать - кто работает, а кто нет в шейдере...)
uniform sampler2D tex1, tex2;
uniform int NeedMultitexture;

// тянем нормаль (уже нормализованную)
varying vec3 Normal;
// тянем вертекс
varying vec3 Vertex;


void main()
{
	vec3 halfV;
	float NdotL, NdotHV;

	// глобальный эмбиент учитываем сразу
	vec4 color = vec4(0.0,0.0,0.0,1.0);
	color += gl_LightModel.ambient * gl_FrontMaterial.ambient;

	// обрабатываем все источники света
	// делаем directional & point light per pixel (а не directional & point light per vertex)
	// ограничиваем 3-мя источниками 1-2 на directional и 1-2 на point...



	// делаем directional light per pixel
	int i=0;
	// учитываем ambient
	color += gl_FrontMaterial.ambient * gl_LightSource[i].ambient;

	NdotL = max(dot(Normal, normalize(gl_LightSource[i].position.xyz)),0.0);
	if (NdotL > 0.0) 
	{
		color += gl_FrontMaterial.diffuse * gl_LightSource[i].diffuse * NdotL;
	//  specular составляющая
		halfV = normalize(gl_LightSource[i].halfVector.xyz);
		NdotHV = max(dot(Normal, halfV),0.0);
		color += gl_FrontMaterial.specular * gl_LightSource[i].specular * pow(NdotHV, gl_FrontMaterial.shininess);
	}
	



	// делаем point light per pixel
	for (int i=1; i<=4; i++)
	{
		vec3 Direction = vec3(gl_LightSource[i].position.xyz-Vertex);

		NdotL = max(dot(Normal, normalize(Direction)),0.0);
		if (NdotL > 0.0) 
		{
			float Dist = length(Direction);

		// считаем коэф. ослабление источка
			float att = 1.0 / (gl_LightSource[i].constantAttenuation +
					gl_LightSource[i].linearAttenuation * Dist +
					gl_LightSource[i].quadraticAttenuation * Dist * Dist);
		// учитываем diffuse и ambient состовляющие
			color += att * (gl_FrontMaterial.diffuse * gl_LightSource[i].diffuse * NdotL + gl_FrontMaterial.ambient * gl_LightSource[i].ambient);
		//  specular составляющая
			halfV = normalize(gl_LightSource[i].halfVector.xyz);
			NdotHV = max(dot(Normal, halfV),0.0);
			color += att * gl_FrontMaterial.specular * gl_LightSource[i].specular * pow(NdotHV,gl_FrontMaterial.shininess);
		}
	}





	// смотрим, если на tex2 
	if (NeedMultitexture == 0)
		gl_FragColor = clamp(color,0.0,1.0)*texture2D(tex1,gl_TexCoord[0].st);
	else
		gl_FragColor = clamp(clamp(color,0.0,1.0)*texture2D(tex1,gl_TexCoord[0].st)+texture2D(tex2,gl_TexCoord[0].st), 0.0, 1.0);
}
