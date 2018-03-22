/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2018 Mikhail Kurinnoi, Viewizard


	AstroMenace is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	AstroMenace is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// TODO translate comments
// TODO particle system and particle system manager sources should be merged,
//      make sure, that all local scope variables moved under unnamed namespace

#include "../camera/camera.h"
#include "../graphics/graphics.h"
#include "../math/math.h"
#include "particle_system.h"

namespace {

cParticleSystem *StartParticleSystem{nullptr};
cParticleSystem *EndParticleSystem{nullptr};

sGLSL *ParticleSystemGLSL{nullptr};
int ParticleSystemUniformLocations[10];

} // unnamed namespace

bool ParticleSystemUseGLSL{false};
float ParticleSystemQuality{1.0f};


//-----------------------------------------------------------------------------
//	Установка общих состояний
//-----------------------------------------------------------------------------
void vw_InitParticleSystems(bool UseGLSL, float Quality)
{
	ParticleSystemUseGLSL = UseGLSL;
	ParticleSystemQuality = Quality;

	// при первом присоединении устанавливаем шейдер
	if (ParticleSystemUseGLSL) {
		ParticleSystemGLSL = vw_FindShaderByName("ParticleSystem");
		ParticleSystemUniformLocations[0] = vw_GetUniformLocation(ParticleSystemGLSL, "ParticleTexture");
		ParticleSystemUniformLocations[1] = vw_GetUniformLocation(ParticleSystemGLSL, "CameraPoint");
	}
}

//-----------------------------------------------------------------------------
//	Присоеденяем ParticleSystem к списку
//-----------------------------------------------------------------------------
void vw_AttachParticleSystem(cParticleSystem *NewParticleSystem)
{
	if (NewParticleSystem == nullptr)
		return;

	// первый в списке...
	if (EndParticleSystem == nullptr) {
		NewParticleSystem->Prev = nullptr;
		NewParticleSystem->Next = nullptr;
		StartParticleSystem = NewParticleSystem;
		EndParticleSystem = NewParticleSystem;
	} else { // продолжаем заполнение...
		NewParticleSystem->Prev = EndParticleSystem;
		NewParticleSystem->Next = nullptr;
		EndParticleSystem->Next = NewParticleSystem;
		EndParticleSystem = NewParticleSystem;
	}
}

//-----------------------------------------------------------------------------
//	Удаляем ParticleSystem из списка
//-----------------------------------------------------------------------------
void vw_DetachParticleSystem(cParticleSystem *OldParticleSystem)
{
	if (OldParticleSystem == nullptr)
		return;

	// переустанавливаем указатели...
	if (StartParticleSystem == OldParticleSystem)
		StartParticleSystem = OldParticleSystem->Next;
	if (EndParticleSystem == OldParticleSystem)
		EndParticleSystem = OldParticleSystem->Prev;

	if (OldParticleSystem->Next != nullptr)
		OldParticleSystem->Next->Prev = OldParticleSystem->Prev;
	else if (OldParticleSystem->Prev != nullptr)
		OldParticleSystem->Prev->Next = nullptr;

	if (OldParticleSystem->Prev != nullptr)
		OldParticleSystem->Prev->Next = OldParticleSystem->Next;
	else if (OldParticleSystem->Next != nullptr)
		OldParticleSystem->Next->Prev = nullptr;
}

//-----------------------------------------------------------------------------
//	Удаляем все ParticleSystem в списке
//-----------------------------------------------------------------------------
void vw_ReleaseAllParticleSystems()
{
	// для всех ParticleSystem
	cParticleSystem *tmp = StartParticleSystem;
	while (tmp) {
		cParticleSystem *tmp2 = tmp->Next;
		// удаляем и очищаем всю память, в релизе стоит DetachShip
		delete tmp;
		tmp = tmp2;
	}

	StartParticleSystem = nullptr;
	EndParticleSystem = nullptr;

	ParticleSystemUseGLSL = false;
	ParticleSystemQuality = 1.0f;
	ParticleSystemGLSL = nullptr;
}

//-----------------------------------------------------------------------------
//	Прорисовываем все ParticleSystems
//-----------------------------------------------------------------------------
void vw_DrawAllParticleSystems()
{
	// текущая текстура
	sTexture *CurrentTexture{nullptr};

	// делаем все предустановки для прорисовки частиц, чтобы не менять каждый раз
	vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_ONE);
	// включаем шейдер, если есть возможность
	if (ParticleSystemUseGLSL && ParticleSystemGLSL) {
		// получаем текущее положение камеры
		sVECTOR3D CurrentCameraLocation;
		vw_GetCameraLocation(&CurrentCameraLocation);

		vw_UseShaderProgram(ParticleSystemGLSL);
		vw_Uniform1i(ParticleSystemGLSL, ParticleSystemUniformLocations[0], 0);
		vw_Uniform3f(ParticleSystemGLSL, ParticleSystemUniformLocations[1],
			     CurrentCameraLocation.x, CurrentCameraLocation.y, CurrentCameraLocation.z);
	}
	// выключаем изменение буфера глубины
	glDepthMask(GL_FALSE);

	// для всех
	cParticleSystem *tmp = StartParticleSystem;
	while (tmp) {
		cParticleSystem *tmp2 = tmp->Next;
		tmp->Draw(&CurrentTexture);
		tmp = tmp2;
	}

	// включаем запись в буфер глубины
	glDepthMask(GL_TRUE);
	// выключаем шейдер
	if (ParticleSystemUseGLSL)
		vw_StopShaderProgram();

	vw_SetTextureBlend(false, 0, 0);
	// анбиндим текстуру
	vw_BindTexture(0, 0);
}

//-----------------------------------------------------------------------------
//	Прорисовываем блок ParticleSystems
//-----------------------------------------------------------------------------
void vw_DrawParticleSystems(cParticleSystem **DrawParticleSystem, int Quantity)
{
	if (!DrawParticleSystem)
		return;

	// текущая текстура
	sTexture *CurrentTexture{nullptr};

	// делаем все предустановки для прорисовки частиц, чтобы не менять каждый раз
	vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_ONE);
	// включаем шейдер, если есть возможность
	if (ParticleSystemUseGLSL && ParticleSystemGLSL) {
		// получаем текущее положение камеры
		sVECTOR3D CurrentCameraLocation;
		vw_GetCameraLocation(&CurrentCameraLocation);

		vw_UseShaderProgram(ParticleSystemGLSL);
		vw_Uniform1i(ParticleSystemGLSL, ParticleSystemUniformLocations[0], 0);
		vw_Uniform3f(ParticleSystemGLSL, ParticleSystemUniformLocations[1],
			     CurrentCameraLocation.x, CurrentCameraLocation.y, CurrentCameraLocation.z);
	}
	// выключаем изменение буфера глубины
	glDepthMask(GL_FALSE);

	for (int i = 0; i < Quantity; i++) {
		if (DrawParticleSystem[i])
			DrawParticleSystem[i]->Draw(&CurrentTexture);
	}

	// включаем запись в буфер глубины
	glDepthMask(GL_TRUE);
	// выключаем шейдер
	if (ParticleSystemUseGLSL)
		vw_StopShaderProgram();

	vw_SetTextureBlend(false, 0, 0);
	// анбиндим текстуру
	vw_BindTexture(0, 0);
}

//-----------------------------------------------------------------------------
//	Апдейтим все ParticleSystems
//-----------------------------------------------------------------------------
void vw_UpdateAllParticleSystems(float Time)
{
	// для всех
	cParticleSystem *tmp = StartParticleSystem;
	while (tmp) {
		cParticleSystem *tmp2 = tmp->Next;
		if (!tmp->Update(Time))
			delete tmp;
		tmp = tmp2;
	}
}
