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


#include "light.h"
#include "../graphics/graphics.h"



sLight * StartLight = nullptr;
sLight * EndLight = nullptr;



//-----------------------------------------------------------------------------
//	Присоеденяем sLight к списку
//-----------------------------------------------------------------------------
void vw_AttachLight(sLight * NewLight)
{
	if (NewLight == nullptr)
		return;

	if (EndLight == nullptr) {
		NewLight->Prev = nullptr;
		NewLight->Next = nullptr;
		StartLight = NewLight;
		EndLight = NewLight;
	} else {
		NewLight->Prev = EndLight;
		NewLight->Next = nullptr;
		EndLight->Next = NewLight;
		EndLight = NewLight;
	}

}




//-----------------------------------------------------------------------------
//	Удаляем sLight из списка
//-----------------------------------------------------------------------------
void vw_DetachLight(sLight * OldLight)
{
	if (OldLight == nullptr)
		return;

	if (StartLight == OldLight)
		StartLight = OldLight->Next;
	if (EndLight == OldLight)
		EndLight = OldLight->Prev;

	if (OldLight->Next != nullptr)
		OldLight->Next->Prev = OldLight->Prev;
	else if (OldLight->Prev != nullptr)
		OldLight->Prev->Next = nullptr;

	if (OldLight->Prev != nullptr)
		OldLight->Prev->Next = OldLight->Next;
	else if (OldLight->Next != nullptr)
		OldLight->Next->Prev = nullptr;
}





//-----------------------------------------------------------------------------
// находим кол-во точечных источников
//-----------------------------------------------------------------------------
int	vw_CheckAllPointLights(sVECTOR3D Location, float Radius2)
{
	float AttLimit = 10.0f; // граница, после которой источник уже не воспринимаем

	int CurrentLightCount = 0;


	// вычисляем воздействие
	sLight *tmp = StartLight;
	while (tmp != nullptr) {
		sLight *tmp2 = tmp->Next;

		if ((tmp->LightType == 1) && tmp->On) { // только включенный!
			// находим квадрат расстояния от центра до источника
			sVECTOR3D DistV = Location - tmp->Location;
			float Dist2 = DistV.x*DistV.x + DistV.y*DistV.y + DistV.z*DistV.z;
			// учитываем радиус объекта
			if (Dist2 > Radius2) Dist2 -= Radius2;
			else Dist2 = 0.0f;
			// считаем без линейной состовляющей
			float tmpAttenuation = tmp->ConstantAttenuation + tmp->QuadraticAttenuation*Dist2;

			if (tmpAttenuation <= AttLimit) {
				// если она есть...
				if (tmp->LinearAttenuation != 0.0f) {
					// достаточно близко, надо учесть линейную составляющую
					float Dist = vw_sqrtf(Dist2);
					tmpAttenuation += tmp->LinearAttenuation*Dist;

					if (tmpAttenuation <= AttLimit)
						CurrentLightCount++;
				} else {
					CurrentLightCount++;
				}
			}
		}
		tmp = tmp2;
	}


	return CurrentLightCount;
}





//-----------------------------------------------------------------------------
//	Включаем нужное освещение
//-----------------------------------------------------------------------------
int vw_CheckAndActivateAllLights(int *Type1, int *Type2, sVECTOR3D Location, float Radius2, int DirLimit, int PointLimit, float Matrix[16])
{
	// текущий источник света
	int CurrentLight = 0;
	float AttLimit = 10.0f; // граница, после которой источник уже не воспринимаем
	*Type2 = 0;
	*Type1 = 0;

	// всегда первыми ставим источники типа солнца (направленный свет)
	sLight *tmp = StartLight;
	while (tmp != nullptr) {
		sLight *tmp2 = tmp->Next;

		if ((tmp->LightType == 0) && tmp->Activate(CurrentLight, Matrix)) {
			CurrentLight++;
			*Type1+=1;
			if (CurrentLight >= vw_GetDevCaps()->MaxActiveLights)
				goto onlight;
			if (*Type1 > DirLimit)
				break;
		}

		// используем это как первый проход и делаем сброс
		tmp->tmpAttenuation = -1.0;

		tmp = tmp2;
	}



	// остальные источники
	if (PointLimit > 0) {
		// вычисляем воздействие
		tmp = StartLight;
		while (tmp != nullptr) {
			sLight *tmp2 = tmp->Next;
			if ((tmp->LightType == 1) && tmp->On) { // только включенный!
				// находим квадрат расстояния от центра до источника
				sVECTOR3D DistV = Location - tmp->Location;
				float Dist2 = DistV.x*DistV.x + DistV.y*DistV.y + DistV.z*DistV.z;
				// учитываем радиус объекта
				if (Dist2 > Radius2)
					Dist2 -= Radius2;
				else
					Dist2 = 0.0f;
				// считаем без линейной состовляющей
				tmp->tmpAttenuation = tmp->ConstantAttenuation + tmp->QuadraticAttenuation*Dist2;

				if (tmp->tmpAttenuation > AttLimit) {
					// слишком далеко...
					tmp->tmpAttenuation = -1.0f;
				} else {
					// если она есть...
					if (tmp->LinearAttenuation != 0.0f) {
						// достаточно близко, надо учесть линейную составляющую
						float Dist = vw_sqrtf(Dist2);
						tmp->tmpAttenuation += tmp->LinearAttenuation*Dist;

						if (tmp->tmpAttenuation > AttLimit)
							// слишком далеко...
							tmp->tmpAttenuation = -1.0f;
					}
				}
			}
			tmp = tmp2;
		}




		// находим те, которые максимально воздействуют на объект
		// делаем перебор по списку нужное кол-во раз
		for (int i=0; i<PointLimit; i++) {
			sLight *tmpCurrent = nullptr; // текущая минимальная
			tmp = StartLight;
			while (tmp != nullptr) {
				sLight *tmp2 = tmp->Next;

				if ((tmp->LightType == 1) && tmp->On) { // только включенный!
					if (tmpCurrent == nullptr) {
						if (tmp->tmpAttenuation != -1.0f)
							tmpCurrent = tmp;
					} else {
						if ((tmp->tmpAttenuation != -1.0f) &&
						    (tmpCurrent->tmpAttenuation > tmp->tmpAttenuation))
							tmpCurrent = tmp;
					}
				}
				tmp = tmp2;
			}


			// включаем то, что нашли (если что-то есть)
			if ((tmpCurrent != nullptr) && tmpCurrent->Activate(CurrentLight, Matrix)) {
				CurrentLight++;
				*Type2+=1;
				// делаем сброс, чтобы не выбрать 2 раза один и тот же
				tmpCurrent->tmpAttenuation = -1.0f;
				if (CurrentLight >= vw_GetDevCaps()->MaxActiveLights)
					goto onlight;
				if (*Type2 > PointLimit)
					goto onlight;
			} else {
				goto onlight; // ничего не нашли, значит там ничего нет и дальше
			}
		}
	}


onlight:
	vw_Lighting(true);
	return CurrentLight;
}






//-----------------------------------------------------------------------------
//	Выключаем что было включено
//-----------------------------------------------------------------------------
void vw_DeActivateAllLights()
{
	// для всех Light
	sLight *tmp = StartLight;
	while (tmp != nullptr) {
		sLight *tmp2 = tmp->Next;
		tmp->DeActivate();
		tmp = tmp2;
	}

	vw_Lighting(false);
}





//-----------------------------------------------------------------------------
// Удаляем источник света
//-----------------------------------------------------------------------------
void vw_ReleaseLight(sLight *Light)
{
	if (Light != nullptr)
		delete Light;
}


//-----------------------------------------------------------------------------
//	Удаляем все sLight в списке
//-----------------------------------------------------------------------------
void vw_ReleaseAllLights()
{
	// для всех Light
	sLight *tmp = StartLight;
	while (tmp != nullptr) {
		sLight *tmp2 = tmp->Next;
		vw_ReleaseLight(tmp);
		tmp = tmp2;
	}

	StartLight = nullptr;
	EndLight = nullptr;
}







//-----------------------------------------------------------------------------
// Создаем точечный источник света
//-----------------------------------------------------------------------------
sLight *vw_CreatPointLight(sVECTOR3D Location, float R, float G, float B, float Linear, float Quadratic)
{
	// создаем источник
	sLight *Light = new sLight;

	// заполняем данными

	Light->LightType = 1;

	Light->Diffuse[0] = Light->Specular[0] = R;
	Light->Diffuse[1] = Light->Specular[1] = G;
	Light->Diffuse[2] = Light->Specular[2] = B;
	Light->Diffuse[3] = Light->Specular[3] = 1.0f;


	Light->LinearAttenuation = Light->LinearAttenuationBase = Linear;
	Light->QuadraticAttenuation = Light->QuadraticAttenuationBase = Quadratic;

	Light->Location = Location;

	return Light;
}




//-----------------------------------------------------------------------------
// Создаем точечный источник света
//-----------------------------------------------------------------------------
sLight 	*vw_GetMainDirectLight()
{
	// перебираем все по порядку
	sLight *tmp = StartLight;
	while (tmp != nullptr) {
		sLight *tmp2 = tmp->Next;
		if (tmp->LightType == 0)
			// первый в списке всегда "основной"
			return tmp;

		tmp = tmp2;
	}

	// нет вообще направленного источника
	return nullptr;
}

