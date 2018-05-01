#version 120

// particle system

uniform vec3 CameraPoint;

varying vec4 VertexColor;


void main(void)
{ 
	// texture's V coord. used as particle size data storage
	float ParticleSize = gl_MultiTexCoord0[1];

	vec3 nnTmp = normalize(CameraPoint - gl_Vertex.xyz);

	// texture's U coord. used as particle number storage
	float VertexNum = gl_MultiTexCoord0[0];

	vec3 nnTmp2 = normalize(vec3(1.0, 1.0, -(nnTmp[0] + nnTmp[1]) / nnTmp[2]));

	vec4 Vertex;

	// calculate vertex position according to its number, in this way we calculate
	// particle rotation perpendicular to eyes point (in shader)
	// since we used texture's coord., restore proper texture's coord. values
	if ((VertexNum == 1.0) || (VertexNum == 3.0)) {
		vec3 nnTmp3 = cross(nnTmp, nnTmp2);
		nnTmp3 = normalize(nnTmp3);

		if (VertexNum == 1.0) {
			Vertex = gl_Vertex + vec4(nnTmp3 * -ParticleSize * 1.5, 0.0);
			gl_TexCoord[0][0] = 0.0;
			gl_TexCoord[0][1] = 1.0;
		} else {
			// VertexNum == 3.0
			Vertex = gl_Vertex + vec4(nnTmp3 * ParticleSize * 1.5, 0.0);
			gl_TexCoord[0][0] = 1.0;
			gl_TexCoord[0][1] = 0.0;
		}
	} else {
		if (VertexNum == 2.0) {
			Vertex = gl_Vertex + vec4(nnTmp2 * ParticleSize * 1.5, 0.0);
			gl_TexCoord[0][0] = 0.0;
			gl_TexCoord[0][1] = 0.0;
		} else {
			// VertexNum == 4.0
			Vertex = gl_Vertex + vec4(nnTmp2 * -ParticleSize * 1.5, 0.0);
			gl_TexCoord[0][0] = 1.0;
			gl_TexCoord[0][1] = 1.0;
		}
	}

	VertexColor = gl_Color;
	gl_Position = gl_ModelViewProjectionMatrix * Vertex;
}
