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

#ifndef MODEL3D_H
#define MODEL3D_H

#include "../base.h"

struct sVECTOR3D;

struct sObjectBlock {
	~sObjectBlock(void);

	int VertexFormat{0};	// формат вертексов данных
	int VertexStride{0};	// отступ в (байтах*4, т.е. во float или uint32_t) до начала данных следующей точки
	int VertexCount{0};	// кол-во прорисовываемых вертексов (кол-во обрабатываемых индексов)
	uint8_t DrawType{0};	// тип прорисовки, 0- нормальный, 1-blend т.е. с прозрачностью

	// начальный номер индекса в буфере (в случае использования глобальных буферов)
	// если индексных данных нет - номер вертекса
	unsigned int RangeStart{0};

	// шейдер (зарезервированные переменные для работы внешнего блока кода)
	int ShaderType{1};
	float ShaderData[16]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	// положение локальной системы координат
	sVECTOR3D Location{0.0f, 0.0f, 0.0f};
	// поворот модели в локальной системе координат
	sVECTOR3D Rotation{0.0f, 0.0f, 0.0f};

	// анимация геометрией (вращение)
	bool NeedGeometryAnimation{false};
	sVECTOR3D GeometryAnimation{0.0f, 0.0f, 0.0f};
	// анимация текстуры (тайловая анимация)
	bool NeedTextureAnimation{false};
	sVECTOR3D TextureAnimation{0.0f, 0.0f, 0.0f};

	bool NeedDestroyDataInObjectBlock{false}; // если данные были не общие, а созданные для этого ObjectBlock, нужно их удалить в деструкторе
	float *VertexArray{nullptr};		// массив вертексов
	unsigned int VBO{0};			// номер VBO
	unsigned int *IndexArray{nullptr};	// массив индексов
	unsigned int IBO{0};			// номер IBO
	// VAO
	unsigned int VAO{0};			// номер VAO
	// указатель на буфер, с мешем приведенным к опеределнному размеру треугольников (для взрывов)
	// для него никогда не делаем VBO, буфер исключительно для работы внутри программы
	float *VertexBufferLimitedBySizeTriangles{nullptr};
	int VertexBufferLimitedBySizeTrianglesCount{0}; // кол-во вертексов в нем
};

//-----------------------------------------------------------------------------
// Класс cModel3D
//-----------------------------------------------------------------------------
class cModel3D
{
public:
	cModel3D(void);
	virtual ~cModel3D(void);

	// имя объекта (путь к файлу)
	std::string Name{};

	// лист объектов, из которых состоит модель
	sObjectBlock *DrawObjectList{nullptr};
	int DrawObjectCount{0};
	unsigned int GlobalVertexCount{0};	// фактическое кол-во вертексов в буфере
	unsigned int GlobalIndexCount{0};	// фактическое кол-во индексов в буфере

	// читаем-пишем форматы 3д моделей
	bool ReadVW3D(const std::string &FileName);
	bool WriteVW3D(const std::string &FileName);

	// пересоздаем вертексный буфер, добавляем тангент и бинормаль в 2 и 3 текстурные координаты
	void CreateTangentAndBinormal();
	// создание вертекс и индекс буферов для каждого блока модели
	void CreateObjectsBuffers();
	// создание всех поддерживаемых буферов (VAO, VBO, IBO)
	void CreateHardwareBuffers();
	// создаем буфер для разрушаемых объектов с ограничением по размеру треугольников
	void CreateVertexBufferLimitedBySizeTriangles(float TriangleSizeLimit);

	// буферы
	float *GlobalVertexArray{nullptr};
	unsigned int GlobalVBO{0};
	unsigned int *GlobalIndexArray{nullptr};
	unsigned int GlobalIBO{0};
	unsigned int GlobalVAO{0};

	// указатели на цепь моделей
	cModel3D *Next{nullptr};
	cModel3D *Prev{nullptr};
};


// Предварительная загрузка геометрии модели, если нужно создания доп. буфера с треугольниками не более TriangleSizeLimit
// если не нужно, передаем отрицательное значение
cModel3D *vw_LoadModel3D(const std::string &FileName, float TriangleSizeLimit, bool NeedTangentAndBinormal);
// Удаляем все Model3D в списке
void vw_ReleaseAllModel3D();

#endif // MODEL3D_H
