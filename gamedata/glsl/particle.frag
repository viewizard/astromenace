#version 120

// particle system

uniform sampler2D ParticleTexture;

varying vec4 VertexColor;


void main(void)
{
	vec2 TexCoord = vec2(gl_TexCoord[0]);
	vec4 TexColor = texture2D(ParticleTexture, TexCoord);
	gl_FragColor = TexColor * VertexColor;
}


