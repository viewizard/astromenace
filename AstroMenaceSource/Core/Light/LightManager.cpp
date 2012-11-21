/********************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2012 Michael Kurinnoy, Viewizard


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


	Web Site:		http://www.viewizard.com/
	E-mail:			viewizard@viewizard.com
	Postal address:	kvartal Zhukova, 10/79, Lugansk city, 91051, Ukraine


*********************************************************************************/


#include "Light.h"
#include "../RendererInterface/RendererInterface.h"



eLight * StartLight = 0;
eLight * EndLight = 0;



//-----------------------------------------------------------------------------
//	Присоеденяем eLight к списку
//-----------------------------------------------------------------------------
void vw_AttachLight(eLight * NewLight)
{
	if (NewLight == 0) return;

	// первый в списке...
	if (EndLight == 0)
	{
		NewLight->Prev = 0;
		NewLight->Next = 0;
		StartLight = NewLight;
		EndLight = NewLight;
	}
	else // продолжаем заполнение...
	{
		NewLight->Prev = EndLight;
		NewLight->Next = 0;
		EndLight->Next = NewLight;
		EndLight = NewLight;
	}

}




//-----------------------------------------------------------------------------
//	Удаляем eLight из списка
//-----------------------------------------------------------------------------
void vw_DetachLight(eLight * OldLight)
{
	if (OldLight == 0) return;

	// переустанавливаем указатели...
	if (StartLight == OldLight) StartLight = OldLight->Next;
	if (EndLight == OldLight) EndLight = OldLight->Prev;


	if (OldLight->Next != 0) OldLight->Next->Prev = OldLight->Prev;
		else if (OldLight->Prev != 0) OldLight->Prev->Next = 0;
	if (OldLight->Prev != 0) OldLight->Prev->Next = OldLight->Next;
		else if (OldLight->Next != 0) OldLight->Next->Prev = 0;
}





//-----------------------------------------------------------------------------
// находим кол-во точечных источников
//-----------------------------------------------------------------------------
int	vw_CheckAllPointLights(VECTOR3D Location, float Radius2)
{
	float AttLimit = 10.0f; // граница, после которой источник уже не воспринимаем

	int CurrentLightCount = 0;


	// вычисляем воздействие
	eLight *tmp = StartLight;
	while (tmp!=0)
	{
		eLight *tmp2 = tmp->Next;
		if (tmp->LightType == 1)
		if (tmp->On) // только включенный!
		{
			// находим квадрат расстояния от центра до источника
			VECTOR3D DistV = Location - tmp->Location;
			float Dist2 = DistV.x*DistV.x + DistV.y*DistV.y + DistV.z*DistV.z;
			// учитываем радиус объекта
			if (Dist2 > Radius2) Dist2 -= Radius2;
			else Dist2 = 0.0f;
			// считаем без линейной состовляющей
			float tmpAttenuation = tmp->ConstantAttenuation + tmp->QuadraticAttenuation*Dist2;

			if (tmpAttenuation > AttLimit)
			{
				// слишком далеко...
			}
			else
			{
				// если она есть...
				if (tmp->LinearAttenuation != 0.0f)
				{
					// достаточно близко, надо учесть линейную составляющую
					float Dist = vw_sqrtf(Dist2);
					tmpAttenuation += tmp->LinearAttenuation*Dist;

					if (tmpAttenuation > AttLimit)
					{
						// слишком далеко...
					}
					else
					{
						CurrentLightCount++;
					}
				}
				else
				{
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
int vw_CheckAndActivateAllLights(int *Type1, int *Type2, VECTOR3D Location, float Radius2, int DirLimit, int PointLimit, float Matrix[16])
{
	// текущий источник света
	int CurrentLight = 0;
	float AttLimit = 10.0f; // граница, после которой источник уже не воспринимаем
	*Type2 = 0;
	*Type1 = 0;

	eDevCaps *CAPS = vw_GetDevCaps();

	// всегда первыми ставим источники типа солнца (направленный свет)
	eLight *tmp = StartLight;
	while (tmp!=0)
	{
		eLight *tmp2 = tmp->Next;
		if (tmp->LightType == 0)
		{
			if (tmp->Activate(CurrentLight, Matrix))
			{
				CurrentLight++;
				*Type1+=1;
				if (CurrentLight >= CAPS->MaxActiveLights) goto onlight;
				if (*Type1 > DirLimit) break;
			}
		}

		// используем это как первый проход и делаем сброс
		tmp->tmpAttenuation = -1.0;

		tmp = tmp2;
	}



	// остальные источники
	if (PointLimit > 0)
	{

		// вычисляем воздействие
		tmp = StartLight;
		while (tmp!=0)
		{
			eLight *tmp2 = tmp->Next;
			if (tmp->LightType == 1)
			if (tmp->On) // только включенный!
			{
				// находим квадрат расстояния от центра до источника
				VECTOR3D DistV = Location - tmp->Location;
				float Dist2 = DistV.x*DistV.x + DistV.y*DistV.y + DistV.z*DistV.z;
				// учитываем радиус объекта
				if (Dist2 > Radius2) Dist2 -= Radius2;
				else Dist2 = 0.0f;
				// считаем без линейной состовляющей
				tmp->tmpAttenuation = tmp->ConstantAttenuation + tmp->QuadraticAttenuation*Dist2;

				if (tmp->tmpAttenuation > AttLimit)
				{
					// слишком далеко...
					tmp->tmpAttenuation = -1.0f;
				}
				else
				{
					// если она есть...
					if (tmp->LinearAttenuation != 0.0f)
					{
						// достаточно близко, надо учесть линейную составляющую
						float Dist = vw_sqrtf(Dist2);
						tmp->tmpAttenuation += tmp->LinearAttenuation*Dist;

						if (tmp->tmpAttenuation > AttLimit)
						{
							// слишком далеко...
							tmp->tmpAttenuation = -1.0f;
						}
					}
				}
			}
			tmp = tmp2;
		}




		// находим те, которые максимально воздействуют на объект
		// делаем перебор по списку нужное кол-во раз
		for (int i=0; i<PointLimit; i++)
		{

			eLight *tmpCurrent = 0; // текущая минимальная
			tmp = StartLight;
			while (tmp!=0)
			{
				eLight *tmp2 = tmp->Next;
				if (tmp->LightType == 1)
				if (tmp->On) // только включенный!
				{
					if (tmpCurrent == 0)
					{
						if (tmp->tmpAttenuation != -1.0f) tmpCurrent = tmp;
					}
					else
					{
						if (tmp->tmpAttenuation != -1.0f)
							if (tmpCurrent->tmpAttenuation > tmp->tmpAttenuation) tmpCurrent = tmp;
					}
				}
				tmp = tmp2;
			}


			// включаем то, что нашли (если что-то есть)
			if (tmpCurrent != 0)
			{
				if (tmpCurrent->Activate(CurrentLight, Matrix))
				{
					CurrentLight++;
					*Type2+=1;
					// делаем сброс, чтобы не выбрать 2 раза один и тот же
					tmpCurrent->tmpAttenuation = -1.0f;
					if (CurrentLight >= CAPS->MaxActiveLights) goto onlight;
					if (*Type2 > PointLimit) goto onlight;
				}
			}
			else goto onlight; // ничего не нашли, значит там ничего нет и дальше

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
	eLight *tmp = StartLight;
	while (tmp!=0)
	{
		eLight *tmp2 = tmp->Next;
		tmp->DeActivate();
		tmp = tmp2;
	}

	vw_Lighting(false);
}





//-----------------------------------------------------------------------------
// Удаляем источник света
//-----------------------------------------------------------------------------
void vw_ReleaseLight(eLight *Light)
{
	if (Light != 0) {delete Light; Light = 0;}
}


//-----------------------------------------------------------------------------
//	Удаляем все eLight в списке
//-----------------------------------------------------------------------------
void vw_ReleaseAllLights()
{
	// для всех Light
	eLight *tmp = StartLight;
	while (tmp!=0)
	{
		eLight *tmp2 = tmp->Next;
		vw_ReleaseLight(tmp);
		tmp = tmp2;
	}

	StartLight = 0;
	EndLight = 0;
}







//-----------------------------------------------------------------------------
// Создаем точечный источник света
//-----------------------------------------------------------------------------
eLight *vw_CreatPointLight(VECTOR3D Location, float R, float G, float B, float Linear, float Quadratic)
{
	// создаем источник
	eLight *Light;
	Light = new eLight; if (Light==0) return 0;

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
eLight 	*vw_GetMainDirectLight()
{
	// перебираем все по порядку
	eLight *tmp = StartLight;
	while (tmp!=0)
	{
		eLight *tmp2 = tmp->Next;
		if (tmp->LightType == 0)
		{
			// первый в списке всегда "основной"
			return tmp;
		}

		tmp = tmp2;
	}

	// нет вообще направленного источника
	return 0;
}

