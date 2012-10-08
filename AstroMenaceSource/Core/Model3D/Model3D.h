/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Model3D.h

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


#ifndef MODEL3D_H
#define MODEL3D_H

#include "../Base.h"
#include "../Math/Math.h"
#include "../VirtualFileSystem/VFS.h"






//-----------------------------------------------------------------------------
// структура блока объектов
//-----------------------------------------------------------------------------
struct eObjectBlock
{
	eObjectBlock(void);
	virtual ~eObjectBlock(void);

	int	FVF_Format;	// формат вертексов данных
	int	Stride;		// отступ в (байтах*4, т.е. во float или DWORD) до начала данных следующей точки
	int VertexCount;// кол-во вертексов.
	BYTE DrawType;	// тип прорисовки, 0- нормальный, 1-blend т.е. с прозрачностью

	// индексы, если индексов нет - указатель на номер вертекса
	unsigned int RangeStart;


	// положение локальной системы координат
	VECTOR3D Location;
	// поворот модели в локальной системе координат
	VECTOR3D Rotation;


	bool			NeedDestroyDataInObjectBlock; // если данные были не общие, а созданные для этого ObjectBlock, нужно их удалить в деструкторе
	// вертексный буфер
	float 			*VertexBuffer;	// указатель на структуру данных
	unsigned int 	*VertexBufferVBO; // указатель на номер VBO массива
	// индексный буфер
	unsigned int 	*IndexBuffer;	// указатель на структуру данных
	unsigned int 	*IndexBufferVBO; // указатель на номер VBO массива
	// VAO
	unsigned int *VAO;
};




//-----------------------------------------------------------------------------
// Класс eModel3D
//-----------------------------------------------------------------------------
class eModel3D
{
public:

	eModel3D(void);
	virtual ~eModel3D(void);

	// имя объекта (путь к файлу)
	char	*Name;

	// лист объектов, из которых состоит модель
	eObjectBlock *DrawObjectList;
	int DrawObjectCount;

	void ReadVW3D(const char *nName);

	float *GlobalVertexBuffer;
	unsigned int *GlobalVertexBufferVBO;
	unsigned int *GlobalIndexBuffer;
	unsigned int *GlobalIndexBufferVBO;
	unsigned int *GlobalVAO;

	// указатели на цепь моделей
	eModel3D *Next;
	eModel3D *Prev;
};






//-----------------------------------------------------------------------------
// Менеджер eObject3D
//-----------------------------------------------------------------------------

// Предварительная загрузка геометрии модели
eModel3D *vw_LoadModel3D(const char *nName);
// Присоеденяем Model3D к списку
void vw_AttachModel3D(eModel3D * NewModel3D);
// Удаляем Model3D из списка
void vw_DetachModel3D(eModel3D * OldModel3D);
// Удаляем все Model3D в списке
void vw_ReleaseAllModel3D();






#endif // MODEL3D_H
