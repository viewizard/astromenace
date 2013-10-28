/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2013 Michael Kurinnoy, Viewizard


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
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


#include "Model3D.h"
#include "../System/System.h"


eModel3D *StartModel3D = 0;
eModel3D *EndModel3D = 0;








//-----------------------------------------------------------------------------
// Нахождение геометрии, или ее загрузка
//-----------------------------------------------------------------------------
eModel3D *vw_LoadModel3D(const char *FileName, float TriangleSizeLimit, bool NeedTangentAndBinormal)
{
	// сначала пытаемся найти уже сущ.
	eModel3D *tmp = StartModel3D;
	while (tmp != 0)
	{
		eModel3D *tmp2 = tmp->Next;
		if(!strcmp(tmp->Name, FileName)) return tmp;
		tmp = tmp2;
	}

	// если ничего нет, значит нужно загрузить
	eModel3D * Model = 0;
	Model = new eModel3D;
	if (Model == 0) return 0;



	// проверяем, вообще есть расширение или нет, плюс, получаем указатель на последнюю точку
	const char *file_ext = strrchr(FileName, '.');
	if (file_ext)
	{
		if (!strcasecmp(".vw3d", file_ext))
		{
			if (!Model->ReadVW3D(FileName))
			{
				printf("Can't load file ... %s\n", FileName);
				delete Model;
				return 0;
			}
		}
		else
/*			if (!strcasecmp(".obj", file_ext))
			{
				if (!Model->ReadOBJ(FileName))
				{
					printf("Can't load file ... %s\n", FileName);
					delete Model;
					return 0;
				}
			}
			else
				if (!strcasecmp(".3ds", file_ext))
				{
					if (!Model->Read3DS(FileName))
					{
						printf("Can't load file ... %s\n", FileName);
						delete Model;
						return 0;
					}
				}
				else*/
					{
						printf("Format not supported ... %s\n", FileName);
						delete Model;
						return 0;
					}
	}
	else
	{
		printf("Format not supported ... %s\n", FileName);
		delete Model;
		return 0;
	}

	// пересоздаем буфер вертексов, для работы с нормал меппингом в шейдерах, добавляем тангент и бинормаль
	if (NeedTangentAndBinormal) Model->CreateTangentAndBinormal();
	// создаем вертексные и индексные буферы для каждого блока
	Model->CreateObjectsBuffers();
	// создаем все поддерживаемые буферы (VAO, VBO, IBO)
	Model->CreateHardwareBuffers();

	// делаем спец буфер для разрушаемых объектов
	// (!) используем фиксированную последовательность RI_3f_XYZ | RI_3f_NORMAL | RI_2f_TEX
	// с которой работают взрывы в игре, не делаем универсальную (нет необходимости)
	Model->CreateVertexBufferLimitedBySizeTriangles(TriangleSizeLimit);

	printf("Loaded ... %s\n", FileName);

	return Model;
}



//-----------------------------------------------------------------------------
//	Присоеденяем Model3D к списку
//-----------------------------------------------------------------------------
void vw_AttachModel3D(eModel3D * NewModel3D)
{
	if (NewModel3D == 0) return;

	// первый в списке...
	if (EndModel3D == 0)
	{
		NewModel3D->Prev = 0;
		NewModel3D->Next = 0;
		StartModel3D = NewModel3D;
		EndModel3D = NewModel3D;
	}
	else // продолжаем заполнение...
	{
		NewModel3D->Prev = EndModel3D;
		NewModel3D->Next = 0;
		EndModel3D->Next = NewModel3D;
		EndModel3D = NewModel3D;
	}
}




//-----------------------------------------------------------------------------
//	Удаляем Model3D из списка
//-----------------------------------------------------------------------------
void vw_DetachModel3D(eModel3D * OldModel3D)
{
	if (OldModel3D == 0) return;

	// переустанавливаем указатели...
	if (StartModel3D == OldModel3D) StartModel3D = OldModel3D->Next;
	if (EndModel3D == OldModel3D) EndModel3D = OldModel3D->Prev;


	if (OldModel3D->Next != 0) OldModel3D->Next->Prev = OldModel3D->Prev;
		else if (OldModel3D->Prev != 0) OldModel3D->Prev->Next = 0;
	if (OldModel3D->Prev != 0) OldModel3D->Prev->Next = OldModel3D->Next;
		else if (OldModel3D->Next != 0) OldModel3D->Next->Prev = 0;
}




//-----------------------------------------------------------------------------
//	Удаляем все Model3D в списке
//-----------------------------------------------------------------------------
void vw_ReleaseAllModel3D()
{
	eModel3D *tmp = StartModel3D;
	while (tmp!=0)
	{
		eModel3D *tmp2 = tmp->Next;
		delete tmp;
		tmp = tmp2;
	}

	StartModel3D = 0;
	EndModel3D = 0;
}

