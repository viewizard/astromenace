#version 110

// Вертексный шейдер для системы частиц

// передаем из вертексного шейдера цвет вертекса
varying vec4 VertexColor;
// получаем время жизни системы, чтобы делать расчеты прозрачности частицы
uniform float ParticleAge;


void main(void)
{ 
	// дельта альфы
	float AlphaDelta = gl_Normal.z;


	// считаем альфу
	float CurrentAlpha = 1.0;

	// текущее действие
	float CurrentAction = (ParticleAge*AlphaDelta)/(1.0 - 0.3);
	float iCurrentAction = floor(CurrentAction);
	float ActionSize = CurrentAction - iCurrentAction;

	// находим что делаем, затухание или проявление
	if (iCurrentAction/2.0 == floor(iCurrentAction/2.0))
	{
		// если четное, надо проявлять
		CurrentAlpha = 0.3 + ActionSize*0.7;
	}
	else
	{
		// затухание
		CurrentAlpha = 1.0 - ActionSize*0.7;
	}

	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	// передаем цвет частицы
	VertexColor = vec4(0.8, 0.8, 1.0, CurrentAlpha);
	// передаем положение вертекса
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

