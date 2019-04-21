#version 120

// directional & point light per pixel + geometry modification during explosion

uniform sampler2D Texture1;

varying vec3 Normal; // already normalized
varying vec3 Vertex;

vec4 direct_light(int i)
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

vec4 point_light(int i)
{
	vec3 halfV;
	float NdotL, NdotHV;
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

	vec3 Direction = vec3(gl_LightSource[i].position.xyz - Vertex);

	NdotL = max(dot(Normal, normalize(Direction)),0.0);
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
		NdotHV = max(dot(Normal, halfV),0.0);
		color += att *
			 gl_FrontMaterial.specular *
			 gl_LightSource[i].specular *
			 pow(NdotHV, gl_FrontMaterial.shininess);
	}

	return color;
}

void main()
{
	// global ambient
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
	color += gl_LightModel.ambient * gl_FrontMaterial.ambient;

	// for each light sources, calculate directional & point light per pixel
	color += direct_light(0);
	color += point_light(1);
	color += point_light(2);
	color += point_light(3);
	color += point_light(4);
	color += point_light(5);
	color += point_light(6);

	gl_FragColor = clamp(color, 0.0, 1.0) * texture2D(Texture1, gl_TexCoord[0].st);
}
