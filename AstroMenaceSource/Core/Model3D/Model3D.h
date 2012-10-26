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
	int VertexCount;// кол-во прорисовываемых вертексов (кол-во обрабатываемых индексов)
	BYTE DrawType;	// тип прорисовки, 0- нормальный, 1-blend т.е. с прозрачностью

	// начальный номер индекса в буфере (в случае использования глобальных буферов)
	// если индексных данных нет - номер вертекса
	unsigned int RangeStart;


	// шейдер (зарезервированные переменные для работы внешнего блока кода)
	int				ShaderType;
	float			ShaderData[16];


	// положение локальной системы координат
	VECTOR3D Location;
	// поворот модели в локальной системе координат
	VECTOR3D Rotation;

	// анимация геометрией (вращение)
	bool NeedGeometryAnimation;
	VECTOR3D GeometryAnimation;
	// анимация текстуры (тайловая анимация)
	bool NeedTextureAnimation;
	VECTOR3D TextureAnimation;

	bool			NeedDestroyDataInObjectBlock; // если данные были не общие, а созданные для этого ObjectBlock, нужно их удалить в деструкторе
	// вертексный буфер
	float 			*VertexBuffer;		// указатель на структуру данных
	unsigned int 	*VBO;				// номер VBO
	// индексный буфер
	unsigned int 	*IndexBuffer;		// указатель на структуру данных
	unsigned int 	*IBO;				// номер IBO
	// VAO
	unsigned int	*VAO;				// номер VAO
	// указатель на буфер, с мешем приведенным к опеределнному размеру треугольников (для взрывов)
	// для него никогда не делаем VBO, буфер исключительно для работы внутри программы
	float 			*VertexBufferLimitedBySizeTriangles;
	int 			VertexBufferLimitedBySizeTrianglesCount; // кол-во вертексов в нем
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
	char			*Name;

	// лист объектов, из которых состоит модель
	eObjectBlock	*DrawObjectList;
	int				DrawObjectCount;
	unsigned int	GlobalVertexCount;	// фактическое кол-во вертексов в буфере
	unsigned int	GlobalIndexCount;	// фактическое кол-во индексов в буфере


	// читаем-пишем форматы 3д моделей
	bool ReadVW3D(const char *FileName);
	bool WriteVW3D(const char *FileName);


	// создание вертекс и индекс буферов для каждого блока модели
	void CreateObjectsBuffers();
	// создание всех поддерживаемых буферов (VAO, VBO, IBO)
	void CreateHardwareBuffers();
	// создаем буфер для разрушаемых объектов с ограничением по размеру треугольников
	void CreateVertexBufferLimitedBySizeTriangles(float TriangleSizeLimit);


	// буферы
	float 			*GlobalVertexBuffer;
	unsigned int	*GlobalVBO;
	unsigned int	*GlobalIndexBuffer;
	unsigned int	*GlobalIBO;
	unsigned int	*GlobalVAO;

	// указатели на цепь моделей
	eModel3D *Next;
	eModel3D *Prev;
};






//-----------------------------------------------------------------------------
// Менеджер eObject3D
//-----------------------------------------------------------------------------

// Предварительная загрузка геометрии модели, если нужно создания доп. буфера с треугольниками не более TriangleSizeLimit
// если не нужно, передаем отрицательное значение
eModel3D *vw_LoadModel3D(const char *FileName, float TriangleSizeLimit);
// Присоеденяем Model3D к списку
void vw_AttachModel3D(eModel3D * NewModel3D);
// Удаляем Model3D из списка
void vw_DetachModel3D(eModel3D * OldModel3D);
// Удаляем все Model3D в списке
void vw_ReleaseAllModel3D();






#endif // MODEL3D_H
