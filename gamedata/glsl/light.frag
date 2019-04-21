#version 120

// directional & point light per pixel + normal mapping

uniform sampler2D Texture1, Texture2, NormalMap;
uniform int NeedMultitexture;
uniform int NeedNormalMapping;

varying vec3 pNormal; // already normalized
varying vec3 pTangent;
varying vec3 pBinormal;
varying vec3 Vertex;

vec4 direct_light(int i, vec3 Normal)
{
	vec3 halfV;
	float NdotL, NdotHV;
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
	// ambient
	color += gl_FrontMaterial.ambient * gl_LightSource[i].ambient;

	NdotL = max(dot(Normal, normalize(gl_LightSource[i].position.xyz)), 0.0);
	if (NdotL > 0.0) {
		color += gl_FrontMaterial.diffuse * gl_LightSource[i].diffuse * NdotL;
		// specular
		halfV = normalize(gl_LightSource[i].halfVector.xyz);
		NdotHV = max(dot(Normal, halfV), 0.0);
		color += gl_FrontMaterial.specular *
			 gl_LightSource[i].specular *
			 pow(NdotHV, gl_FrontMaterial.shininess);
	}

	return color;
}

vec4 point_light(int i, vec3 Normal)
{
	vec3 halfV;
	float NdotL, NdotHV;
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

	vec3 Direction = vec3(gl_LightSource[i].position.xyz - Vertex);

	NdotL = max(dot(Normal, normalize(Direction)), 0.0);
	if (NdotL > 0.0) {
		float Dist = length(Direction);

		// calculate attenuations
		float att = 1.0 / (gl_LightSource[i].constantAttenuation +
				   gl_LightSource[i].linearAttenuation * Dist +
				   gl_LightSource[i].quadraticAttenuation * Dist * Dist);
		// diffuse and ambient
		color += att * (gl_FrontMaterial.diffuse * gl_LightSource[i].diffuse * NdotL +
				gl_FrontMaterial.ambient * gl_LightSource[i].ambient);
		// specular
		halfV = normalize(gl_LightSource[i].halfVector.xyz);
		NdotHV = max(dot(Normal, halfV), 0.0);
		color += att *
			 gl_FrontMaterial.specular *
			 gl_LightSource[i].specular *
			 pow(NdotHV, gl_FrontMaterial.shininess);
	}

	return color;
}

void main()
{
	vec3 Normal = pNormal;
	if (NeedNormalMapping == 1) {
		mat3 TBN = mat3(pTangent, pBinormal, pNormal);
		Normal = normalize(TBN * normalize(texture2D(NormalMap, gl_TexCoord[0].st).xyz * 2.0 - 1.0));
	}

	vec3 halfV;
	float NdotL, NdotHV;

	// global ambient
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
	color += gl_LightModel.ambient * gl_FrontMaterial.ambient;

	// for each light sources, calculate directional & point light per pixel
	color += direct_light(0, Normal);
	color += point_light(1, Normal);
	color += point_light(2, Normal);
	color += point_light(3, Normal);
	color += point_light(4, Normal);
	color += point_light(5, Normal);
	color += point_light(6, Normal);

	gl_FragColor = clamp(color, 0.0, 1.0) * texture2D(Texture1,gl_TexCoord[0].st);
	if (NeedMultitexture == 1)
		gl_FragColor = clamp(gl_FragColor + texture2D(Texture2, gl_TexCoord[0].st), 0.0, 1.0);
}
