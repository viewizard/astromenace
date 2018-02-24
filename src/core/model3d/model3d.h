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

#ifndef MODEL3D_H
#define MODEL3D_H

#include "../base.h"
#include "../math/math.h"
#include "../vfs/vfs.h"


//-----------------------------------------------------------------------------
// структура блока объектов
//-----------------------------------------------------------------------------
struct eObjectBlock {
	eObjectBlock(void) {};
	~eObjectBlock(void);

	int	VertexFormat{0};	// формат вертексов данных
	int	VertexStride{0};	// отступ в (байтах*4, т.е. во float или DWORD) до начала данных следующей точки
	int	VertexCount{0};		// кол-во прорисовываемых вертексов (кол-во обрабатываемых индексов)
	BYTE	DrawType{0};		// тип прорисовки, 0- нормальный, 1-blend т.е. с прозрачностью

	// начальный номер индекса в буфере (в случае использования глобальных буферов)
	// если индексных данных нет - номер вертекса
	unsigned int	RangeStart{0};

	// шейдер (зарезервированные переменные для работы внешнего блока кода)
	int		ShaderType{1};
	float		ShaderData[16]{0, 0, 0, 0, 0, 0, 0, 0,
				       0, 0, 0, 0, 0, 0, 0, 0};

	// положение локальной системы координат
	VECTOR3D	Location{0.0f, 0.0f, 0.0f};
	// поворот модели в локальной системе координат
	VECTOR3D	Rotation{0.0f, 0.0f, 0.0f};

	// анимация геометрией (вращение)
	bool		NeedGeometryAnimation{false};
	VECTOR3D	GeometryAnimation{0.0f, 0.0f, 0.0f};
	// анимация текстуры (тайловая анимация)
	bool		NeedTextureAnimation{false};
	VECTOR3D	TextureAnimation{0.0f, 0.0f, 0.0f};

	bool		NeedDestroyDataInObjectBlock{false}; // если данные были не общие, а созданные для этого ObjectBlock, нужно их удалить в деструкторе
	// вертексный буфер
	float 		*VertexBuffer{nullptr};		// указатель на структуру данных
	unsigned int 	*VBO{nullptr};			// номер VBO
	// индексный буфер
	unsigned int 	*IndexBuffer{nullptr};		// указатель на структуру данных
	unsigned int 	*IBO{nullptr};			// номер IBO
	// VAO
	unsigned int	*VAO{nullptr};			// номер VAO
	// указатель на буфер, с мешем приведенным к опеределнному размеру треугольников (для взрывов)
	// для него никогда не делаем VBO, буфер исключительно для работы внутри программы
	float 		*VertexBufferLimitedBySizeTriangles{nullptr};
	int 		VertexBufferLimitedBySizeTrianglesCount{0}; // кол-во вертексов в нем
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
	char		*Name{nullptr};

	// лист объектов, из которых состоит модель
	eObjectBlock	*DrawObjectList{nullptr};
	int		DrawObjectCount{0};
	unsigned int	GlobalVertexCount{0};	// фактическое кол-во вертексов в буфере
	unsigned int	GlobalIndexCount{0};	// фактическое кол-во индексов в буфере

	// читаем-пишем форматы 3д моделей
	bool	ReadVW3D(const char *FileName);
	bool	WriteVW3D(const char *FileName);

	// пересоздаем вертексный буфер, добавляем тангент и бинормаль в 2 и 3 текстурные координаты
	void	CreateTangentAndBinormal();
	// создание вертекс и индекс буферов для каждого блока модели
	void	CreateObjectsBuffers();
	// создание всех поддерживаемых буферов (VAO, VBO, IBO)
	void	CreateHardwareBuffers();
	// создаем буфер для разрушаемых объектов с ограничением по размеру треугольников
	void	CreateVertexBufferLimitedBySizeTriangles(float TriangleSizeLimit);

	// буферы
	float 		*GlobalVertexBuffer{nullptr};
	unsigned int	*GlobalVBO{nullptr};
	unsigned int	*GlobalIndexBuffer{nullptr};
	unsigned int	*GlobalIBO{nullptr};
	unsigned int	*GlobalVAO{nullptr};

	// указатели на цепь моделей
	eModel3D	*Next{nullptr};
	eModel3D	*Prev{nullptr};
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
