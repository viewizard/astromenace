/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: ParticleSystemManager.cpp

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.0

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


#include "ParticleSystem.h"
#include "../Camera/Camera.h"
#include "../RendererInterface/RendererInterface.h"


eParticleSystem * StartParticleSystem = 0;
eParticleSystem * EndParticleSystem = 0;

bool ParticleSystemUseGLSL = false;
float ParticleSystemQuality = 1.0f;
eGLSL *ParticleSystemGLSL = 0;


//-----------------------------------------------------------------------------
//	Установка общих состояний
//-----------------------------------------------------------------------------
void vw_SetParticleSystemStates(bool UseGLSL, float Quality)
{
	ParticleSystemUseGLSL = UseGLSL;
	ParticleSystemQuality = Quality;
}




//-----------------------------------------------------------------------------
//	Присоеденяем ParticleSystem к списку
//-----------------------------------------------------------------------------
void vw_AttachParticleSystem(eParticleSystem * NewParticleSystem)
{
	if (NewParticleSystem == 0) return;

	// первый в списке...
	if (EndParticleSystem == 0)
	{
		NewParticleSystem->Prev = 0;
		NewParticleSystem->Next = 0;
		StartParticleSystem = NewParticleSystem;
		EndParticleSystem = NewParticleSystem;
	}
	else // продолжаем заполнение...
	{
		NewParticleSystem->Prev = EndParticleSystem;
		NewParticleSystem->Next = 0;
		EndParticleSystem->Next = NewParticleSystem;
		EndParticleSystem = NewParticleSystem;
	}

	// при первом присоединении устанавливаем шейдер
	if (ParticleSystemUseGLSL)
		if (ParticleSystemGLSL == 0) ParticleSystemGLSL = vw_FindShaderByName("ParticleSystem");
}




//-----------------------------------------------------------------------------
//	Удаляем ParticleSystem из списка
//-----------------------------------------------------------------------------
void vw_DetachParticleSystem(eParticleSystem * OldParticleSystem)
{
	if (OldParticleSystem == 0) return;

	// переустанавливаем указатели...
	if (StartParticleSystem == OldParticleSystem) StartParticleSystem = OldParticleSystem->Next;
	if (EndParticleSystem == OldParticleSystem) EndParticleSystem = OldParticleSystem->Prev;


	if (OldParticleSystem->Next != 0) OldParticleSystem->Next->Prev = OldParticleSystem->Prev;
		else if (OldParticleSystem->Prev != 0) OldParticleSystem->Prev->Next = 0;
	if (OldParticleSystem->Prev != 0) OldParticleSystem->Prev->Next = OldParticleSystem->Next;
		else if (OldParticleSystem->Next != 0) OldParticleSystem->Next->Prev = 0;
}




//-----------------------------------------------------------------------------
//	Удаляем все ParticleSystem в списке
//-----------------------------------------------------------------------------
void vw_ReleaseAllParticleSystems()
{
	// для всех ParticleSystem
	eParticleSystem *tmp = StartParticleSystem;
	while (tmp!=0)
	{
		eParticleSystem *tmp2 = tmp->Next;
		// удаляем и очищаем всю память, в релизе стоит DetachShip
		delete tmp; tmp = 0;
		tmp = tmp2;
	}

	StartParticleSystem = 0;
	EndParticleSystem = 0;

	ParticleSystemUseGLSL = false;
	ParticleSystemQuality = 1.0f;
	ParticleSystemGLSL = 0;
}








//-----------------------------------------------------------------------------
//	Прорисовываем все ParticleSystems
//-----------------------------------------------------------------------------
void vw_DrawAllParticleSystems()
{
	// текущая текстура
	eTexture *CurrentTexture = 0;


	// делаем все предустановки для прорисовки частиц, чтобы не менять каждый раз


	vw_SetTexBlend(RI_BLEND_SRCALPHA, RI_BLEND_ONE);
	// включаем шейдер, если есть возможность
	if (ParticleSystemUseGLSL)
	{
		if (ParticleSystemGLSL != 0)
		{
			// получаем текущее положение камеры
			VECTOR3D CurrentCameraLocation;
			vw_GetCameraLocation(&CurrentCameraLocation);

			vw_UseShaderProgram(ParticleSystemGLSL);
			vw_Uniform1i(ParticleSystemGLSL, "ParticleTexture", 0);
			vw_Uniform3f(ParticleSystemGLSL, "CameraPoint", CurrentCameraLocation.x, CurrentCameraLocation.y, CurrentCameraLocation.z);
		}
	}
	// выключаем изменение буфера глубины
	glDepthMask(GL_FALSE);




	// для всех
	eParticleSystem *tmp = StartParticleSystem;
	while (tmp!=0)
	{
		eParticleSystem *tmp2 = tmp->Next;

		if (CurrentTexture != tmp->Texture)
		{
			vw_SetTextureT(0, tmp->Texture, 1);
			CurrentTexture = tmp->Texture;
		}
		tmp->Draw();

		tmp = tmp2;
	}



	// включаем запись в буфер глубины
	glDepthMask(GL_TRUE);
	// выключаем шейдер
	if (ParticleSystemUseGLSL)
	{
		vw_StopShaderProgram();
	}
	// сбрасываем все установки текстуры
	vw_SetTextureDef(0);
}





//-----------------------------------------------------------------------------
//	Апдейтим все ParticleSystems
//-----------------------------------------------------------------------------
void vw_UpdateAllParticleSystems(float Time)
{
	// для всех
	eParticleSystem *tmp = StartParticleSystem;
	while (tmp!=0)
	{
		eParticleSystem *tmp2 = tmp->Next;
		if (!tmp->Update(Time))
		{
			delete tmp; tmp = 0;
		}
		tmp = tmp2;
	}
}



