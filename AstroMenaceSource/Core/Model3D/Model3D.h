/************************************************************************************

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


	Web Site: http://www.viewizard.com/
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


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

	int	VertexFormat;	// формат вертексов данных
	int	VertexStride;	// отступ в (байтах*4, т.е. во float или DWORD) до начала данных следующей точки
	int VertexCount;	// кол-во прорисовываемых вертексов (кол-во обрабатываемых индексов)
	BYTE DrawType;		// тип прорисовки, 0- нормальный, 1-blend т.е. с прозрачностью

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

	// пересоздаем вертексный буфер, добавляем тангент и бинормаль в 2 и 3 текстурные координаты
	void CreateTangentAndBinormal();
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
eModel3D *vw_LoadModel3D(const char *FileName, float TriangleSizeLimit, bool NeedTangentAndBinormal);
// Присоеденяем Model3D к списку
void vw_AttachModel3D(eModel3D * NewModel3D);
// Удаляем Model3D из списка
void vw_DetachModel3D(eModel3D * OldModel3D);
// Удаляем все Model3D в списке
void vw_ReleaseAllModel3D();






#endif // MODEL3D_H
