#version 120

//  directional & point light per pixel + shadow map with PCF + multitexture

uniform sampler2D Texture1, Texture2;
uniform int NeedMultitexture;

uniform sampler2DShadow ShadowMap;
uniform int ShadowMapStage;

uniform int DirectLightCount;
uniform int PointLightCount;

uniform float xPixelOffset;
uniform float yPixelOffset;

// тянем нормаль (уже нормализованную)
varying vec3 Normal;
// тянем вертекс
varying vec3 Vertex;


float lookup( vec2 offSet)
{
	// Values are multiplied by ShadowCoord.w because shadow2DProj does a W division for us.
	return shadow2DProj(ShadowMap, gl_TexCoord[ShadowMapStage] + vec4(offSet.x * xPixelOffset * gl_TexCoord[ShadowMapStage].w, offSet.y * yPixelOffset * gl_TexCoord[ShadowMapStage].w, 0.0005, 0.0) ).w;
}

void main()
{
	vec3 halfV;
	float NdotL, NdotHV;

	float Shadow = 1.0f;

	// Avoid counter shadow
	if (gl_TexCoord[ShadowMapStage].w > 1.0)
	{
/*
		// Simple lookup, no PCF
		Shadow = lookup(vec2(0.0,0.0));
*/

		// 4x4 kernel PCF
					
		float x,y;
		for (y = -1.5 ; y <=1.5 ; y+=1.0)
			for (x = -1.5 ; x <=1.5 ; x+=1.0)
				Shadow += lookup(vec2(x,y));
		
		Shadow /= 16.0 ;

/*
		// 8x8 kernel PCF
		float x,y;
		for (y = -3.5 ; y <=3.5 ; y+=1.0)
			for (x = -3.5 ; x <=3.5 ; x+=1.0)
				Shadow += lookup(vec2(x,y));
					
		Shadow /= 64.0 ;
*/
/*
		// 4x4  PCF dithered
		// use modulo to vary the sample pattern
		vec2 o = mod(floor(gl_FragCoord.xy), 2.0);
				
		Shadow += lookup(vec2(-1.5, 1.5) + o);
		Shadow += lookup(vec2( 0.5, 1.5) + o);
		Shadow += lookup(vec2(-1.5, -0.5) + o);
		Shadow += lookup(vec2( 0.5, -0.5) + o);
		Shadow *= 0.25 ;
*/
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


	// смотрим, если надо Texture2 
	if (NeedMultitexture == 0)
		gl_FragColor = clamp(color,0.0,1.0)*texture2D(Texture1,gl_TexCoord[0].st);
	else
		gl_FragColor = clamp(clamp(color,0.0,1.0)*texture2D(Texture1,gl_TexCoord[0].st)+texture2D(Texture2,gl_TexCoord[0].st), 0.0, 1.0);
}
