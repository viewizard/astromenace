/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Model3DManager.cpp

	Copyright (c) 2006-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.1

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



	// определяем по расширению что загружать
	if( vw_TestFileExtension( FileName, "vw3d" ) || vw_TestFileExtension( FileName, "VW3D" ))
	{
		if (!Model->ReadVW3D(FileName))
		{
			printf("Can't load file ... %s\n", FileName);
			delete Model;
			return 0;
		}
	}
/*	else
	if( vw_TestFileExtension( FileName, "obj" ) || vw_TestFileExtension( FileName, "OBJ" ))
	{
		Model->ReadOBJ(FileName);
	}
	else
	if( vw_TestFileExtension( FileName, "3ds" ) || vw_TestFileExtension( FileName, "3DS" ))
	{
		Model->Read3DS(FileName);
	}*/
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
		delete tmp; tmp = 0;
		tmp = tmp2;
	}

	StartModel3D = 0;
	EndModel3D = 0;
}

