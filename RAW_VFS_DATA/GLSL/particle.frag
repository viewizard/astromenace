#version 110

// Фрагментный шейдер для системы частиц

// получаем из вертексного шейдера цвет вертекса
varying vec4 VertexColor;
// получаем из основной программы номер текстурного юнита который нужно использовать
uniform sampler2D ParticleTexture;

void main(void)
{
	vec2 TexCoord = vec2(gl_TexCoord[0]);
	vec4 TexColor = texture2D(ParticleTexture, TexCoord);
	gl_FragColor = TexColor*VertexColor;
}


