#version 120

//  directional & point light per pixel + shadow mapping with PCF + normal mapping

uniform sampler2D Texture1, Texture2, NormalMap;
uniform int NeedMultitexture;
uniform int NeedNormalMapping;

uniform sampler2DShadow ShadowMap;
uniform int PCFMode;

uniform int DirectLightCount;
uniform int PointLightCount;

uniform float xPixelOffset;
uniform float yPixelOffset;

// тянем нормаль (уже нормализованную)
varying vec3 pNormal;
varying vec3 pTangent;
varying vec3 pBinormal;

// тянем вертекс
varying vec3 Vertex;
// передаем в фрагментный шейдер данные по точке на карте теней
varying vec4 ShadowTexCoord;


float lookup(vec2 offSet)
{
	// Values are multiplied by ShadowCoord.w because shadow2DProj does a W division for us.
	return shadow2DProj(ShadowMap, ShadowTexCoord + vec4(offSet.x * xPixelOffset * ShadowTexCoord.w, offSet.y * yPixelOffset * ShadowTexCoord.w, 0.0005, 0.0) ).w;
}

void main()
{
	// меняем нормаль с учетом нормал мепа, если нужно
	vec3 Normal = pNormal;
	if (NeedNormalMapping == 1)
	{
		mat3 TBN = mat3(pTangent, pBinormal, pNormal);
		Normal = normalize(TBN * normalize(texture2D(NormalMap, gl_TexCoord[0].st).xyz * 2.0 - 1.0));
	}


	vec3 halfV;
	float NdotL, NdotHV;

	float Shadow = 1.0f;

	// Avoid counter shadow
	if (ShadowTexCoord.w > 1.0)
	{
		if (PCFMode == 8)
		{
			// 8x8 kernel PCF
			float x,y;
			for (y = -3.5 ; y <=3.5 ; y+=1.0)
				for (x = -3.5 ; x <=3.5 ; x+=1.0)
					Shadow += lookup(vec2(x,y));
					
			Shadow /= 64.0 ;
		}
		else
		if (PCFMode == 4)
		{
			// 4x4 kernel PCF		
			float x,y;
			for (y = -1.5 ; y <=1.5 ; y+=1.0)
				for (x = -1.5 ; x <=1.5 ; x+=1.0)
					Shadow += lookup(vec2(x,y));
			
			Shadow /= 16.0 ;
		}
		else
		if (PCFMode == 2)
		{
			// 2x2  PCF dithered
			// use modulo to vary the sample pattern
			vec2 o = mod(floor(gl_FragCoord.xy), 2.0);
					
			Shadow += lookup(vec2(-1.5, 1.5) + o);
			Shadow += lookup(vec2( 0.5, 1.5) + o);
			Shadow += lookup(vec2(-1.5, -0.5) + o);
			Shadow += lookup(vec2( 0.5, -0.5) + o);
			Shadow *= 0.25 ;
		}
		else
		{
			// Simple lookup, no PCF
			Shadow = lookup(vec2(0.0,0.0));
		}		
	}

	// глобальный эмбиент учитываем сразу
	vec4 color = vec4(0.0,0.0,0.0,1.0);
	color += gl_LightModel.ambient * gl_FrontMaterial.ambient;

	// обрабатываем все источники света
	// делаем directional & point light per pixel (а не directional & point light per vertex)

	// делаем directional light per pixel
	for (int i=0; i<DirectLightCount; i++)
	{
		// учитываем ambient
		color += gl_FrontMaterial.ambient * gl_LightSource[i].ambient;

		NdotL = max(dot(Normal, normalize(gl_LightSource[i].position.xyz)),0.0) * Shadow;
		if (NdotL > 0.0) 
		{
			color += gl_FrontMaterial.diffuse * gl_LightSource[i].diffuse * NdotL;
		//  specular составляющая
			halfV = normalize(gl_LightSource[i].halfVector.xyz);
			NdotHV = max(dot(Normal, halfV),0.0);
			color += gl_FrontMaterial.specular * gl_LightSource[i].specular * pow(NdotHV, gl_FrontMaterial.shininess) * Shadow;
		}
	}

	// делаем point light per pixel
	for (int i=DirectLightCount; i<DirectLightCount+PointLightCount; i++)
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


	gl_FragColor = clamp(color,0.0,1.0)*texture2D(Texture1,gl_TexCoord[0].st);
	// смотрим, если надо Texture2
	if (NeedMultitexture == 1)
		gl_FragColor = clamp(gl_FragColor+texture2D(Texture2,gl_TexCoord[0].st), 0.0, 1.0);
}
