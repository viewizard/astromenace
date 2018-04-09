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

// TODO move objects management on STL container
// TODO use std::shared_ptr for memory management
//      std::shared_ptr should be used with arrays and proper deleter, for example:
//      std::shared_ptr<float> array(new float[10], std::default_delete<float[]>());
//      cObject3D should be revised, in order to proper copy struct with containers

#include "../graphics/graphics.h"
#include "../math/math.h"
#include "../vfs/vfs.h"
#include "model3d.h"

namespace {

// All loaded models.
std::unordered_map<std::string, sModel3D> ModelsMap;

} // unnamed namespace


/*
 * Create tangent and binormal for all vertex arrays.
 * In case of GLSL usage, we use second and third textures coordinates in order
 * to store tangent, that we need for normal mapping shader.
 *
 * Note, function expect fixed vertex format for input vertex arrays:
 * RI_3f_XYZ | RI_3f_NORMAL | RI_2f_TEX
 * make sure, you have proper vertex array, before you call this function.
 *
 * Return vertex array with fixed vertex format:
 * RI_3f_XYZ | RI_3f_NORMAL | RI_3_TEX | RI_2f_TEX | RI_SEPARATE_TEX_COORD
 */
static void CreateTangentAndBinormal(sModel3D *Model)
{
	int New_VertexFormat = RI_3f_XYZ | RI_3f_NORMAL | RI_3_TEX | RI_2f_TEX | RI_SEPARATE_TEX_COORD;
	int New_VertexStride = 3 + 3 + 6;
	std::shared_ptr<float> New_VertexBuffer{new float[New_VertexStride * Model->GlobalIndexArrayCount],
						std::default_delete<float[]>()};

	// for vertex array with new size (we need 4 more floats for each vertex), copy
	// data with proper stride, we fill second and third textures coordinates later
	for (unsigned int j = 0; j < Model->GlobalIndexArrayCount; j++) {
		memcpy(New_VertexBuffer.get() + New_VertexStride * j,
		       Model->GlobalVertexArray.get() + Model->GlobalIndexArray.get()[j] * Model->ObjectBlocks[0].VertexStride,
		       Model->ObjectBlocks[0].VertexStride * sizeof(float));
	}
	Model->GlobalVertexArray = New_VertexBuffer;
	Model->GlobalVertexArrayCount = Model->GlobalIndexArrayCount;

	// calculate tangent and binormal
	for (unsigned int j = 0; j < Model->GlobalVertexArrayCount - 2; j += 3) {
		unsigned int tmpOffset0 = New_VertexStride * j;			// j
		unsigned int tmpOffset1 = tmpOffset0 + New_VertexStride;	// j + 1
		unsigned int tmpOffset2 = tmpOffset1 + New_VertexStride;	// j + 2

		sVECTOR3D Point1(Model->GlobalVertexArray.get()[tmpOffset0],
				 Model->GlobalVertexArray.get()[tmpOffset0 + 1],
				 Model->GlobalVertexArray.get()[tmpOffset0 + 2]);

		sVECTOR3D Point2(Model->GlobalVertexArray.get()[tmpOffset1],
				 Model->GlobalVertexArray.get()[tmpOffset1 + 1],
				 Model->GlobalVertexArray.get()[tmpOffset1 + 2]);

		sVECTOR3D Point3(Model->GlobalVertexArray.get()[tmpOffset2],
				 Model->GlobalVertexArray.get()[tmpOffset2 + 1],
				 Model->GlobalVertexArray.get()[tmpOffset2 + 2]);

		// 2 vectors for plane
		sVECTOR3D PlaneVector1 = Point2 - Point1;
		sVECTOR3D PlaneVector2 = Point3 - Point1;
		// plane normal
		PlaneVector1.Multiply(PlaneVector2);
		PlaneVector1.NormalizeHi();
		sVECTOR3D PlaneNormal = PlaneVector1;

		PlaneVector1 = Point1 - Point2;
		PlaneVector2 = Point3 - Point2;

		float delta_U_0 = Model->GlobalVertexArray.get()[tmpOffset0 + 6] -
				  Model->GlobalVertexArray.get()[tmpOffset1 + 6];
		float delta_U_1 = Model->GlobalVertexArray.get()[tmpOffset2 + 6] -
				  Model->GlobalVertexArray.get()[tmpOffset1 + 6];

		float delta_V_0 = Model->GlobalVertexArray.get()[tmpOffset0 + 7] -
				  Model->GlobalVertexArray.get()[tmpOffset1 + 7];
		float delta_V_1 = Model->GlobalVertexArray.get()[tmpOffset2 + 7] -
				  Model->GlobalVertexArray.get()[tmpOffset1 + 7];

		// tangent and binormal
		sVECTOR3D Tangent = ((PlaneVector1 ^ delta_V_1) - (PlaneVector2 ^ delta_V_0));
		Tangent.NormalizeHi();
		float TangentW = 1.0f;
		sVECTOR3D BiNormal = ((PlaneVector1 ^ delta_U_1) - (PlaneVector2 ^ delta_U_0));
		BiNormal.NormalizeHi();

		// check, is normalmap mirrored or not
		sVECTOR3D TBCross = Tangent;
		TBCross.Multiply(BiNormal);
		if( (TBCross * PlaneNormal) < 0 ) {
			// care about mirrored normalmap
			Tangent = ((PlaneVector1 ^ (-delta_V_1)) - (PlaneVector2 ^ (-delta_V_0)));
			Tangent.NormalizeHi();
			TangentW = -1.0f;
		}

		// fill second and third textures coordinates with tangent
		Model->GlobalVertexArray.get()[tmpOffset0 + 8] =
				Model->GlobalVertexArray.get()[tmpOffset1 + 8] =
				Model->GlobalVertexArray.get()[tmpOffset2 + 8] = Tangent.x;
		Model->GlobalVertexArray.get()[tmpOffset0 + 9] =
				Model->GlobalVertexArray.get()[tmpOffset1 + 9] =
				Model->GlobalVertexArray.get()[tmpOffset2 + 9] = Tangent.y;
		Model->GlobalVertexArray.get()[tmpOffset0 + 10] =
				Model->GlobalVertexArray.get()[tmpOffset1 + 10] =
				Model->GlobalVertexArray.get()[tmpOffset2 + 10] = Tangent.z;
		Model->GlobalVertexArray.get()[tmpOffset0 + 11] =
				Model->GlobalVertexArray.get()[tmpOffset1 + 11] =
				Model->GlobalVertexArray.get()[tmpOffset2 + 11] = TangentW;
	}

	// re-generate index array
	for (unsigned int i = 0; i < Model->GlobalIndexArrayCount; i++) {
		Model->GlobalIndexArray.get()[i] = i;
	}

	// store new vertex data
	for (auto &tmpObjectBlock : Model->ObjectBlocks) {
		tmpObjectBlock.VertexArray = Model->GlobalVertexArray;
		tmpObjectBlock.VertexFormat = New_VertexFormat;
		tmpObjectBlock.VertexStride = New_VertexStride;
	}
}

/*
 * Create vertex and index arrays for all objects.
 */
static void CreateObjectsBuffers(sModel3D *Model)
{
	for (auto &tmpObjectBlock : Model->ObjectBlocks) {
		// vertex array
		tmpObjectBlock.VertexArray.reset(new float[tmpObjectBlock.VertexStride * tmpObjectBlock.VertexCount],
						 std::default_delete<float[]>());
		for (unsigned int j = 0; j < tmpObjectBlock.VertexCount; j++) {
			memcpy(tmpObjectBlock.VertexArray.get() + tmpObjectBlock.VertexStride * j,
			       Model->GlobalVertexArray.get() + Model->GlobalIndexArray.get()[tmpObjectBlock.RangeStart + j] *
								tmpObjectBlock.VertexStride,
			       tmpObjectBlock.VertexStride * sizeof(tmpObjectBlock.VertexArray.get()[0]));
		}

		// index array
		tmpObjectBlock.IndexArray.reset(new unsigned[tmpObjectBlock.VertexCount],
						std::default_delete<unsigned[]>());
		for (unsigned int j = 0; j < tmpObjectBlock.VertexCount; j++) {
			tmpObjectBlock.IndexArray.get()[j] = j;
		}
		tmpObjectBlock.RangeStart = 0;
	}
}

/*
 * Create all OpenGL-related hardware buffers.
 */
static void CreateHardwareBuffers(sModel3D *Model)
{
	// global vertex buffer object
	if (!vw_BuildVertexBufferObject(Model->GlobalVertexArrayCount, Model->GlobalVertexArray.get(),
					Model->ObjectBlocks[0].VertexStride, Model->GlobalVBO))
		Model->GlobalVBO = 0;

	// global index buffer object
	if (!vw_BuildIndexBufferObject(Model->GlobalIndexArrayCount, Model->GlobalIndexArray.get(), Model->GlobalIBO))
		Model->GlobalIBO = 0;

	// global vertex array object
	if (!vw_BuildVAO(Model->GlobalVAO, Model->GlobalIndexArrayCount, Model->ObjectBlocks[0].VertexFormat,
			 Model->GlobalVertexArray.get(), Model->ObjectBlocks[0].VertexStride * sizeof(float),
			 Model->GlobalVBO, 0, Model->GlobalIndexArray.get(), Model->GlobalIBO))
		Model->GlobalVAO = 0;

	// and same for all objects
	for (auto &tmpObjectBlock : Model->ObjectBlocks) {
		// vertex buffer object
		if (!vw_BuildVertexBufferObject(tmpObjectBlock.VertexCount, tmpObjectBlock.VertexArray.get(),
						tmpObjectBlock.VertexStride, tmpObjectBlock.VBO))
			tmpObjectBlock.VBO = 0;

		// index buffer object
		if (!vw_BuildIndexBufferObject(tmpObjectBlock.VertexCount, tmpObjectBlock.IndexArray.get(),
					       tmpObjectBlock.IBO))
			tmpObjectBlock.IBO = 0;

		// vertex array object
		if (!vw_BuildVAO(tmpObjectBlock.VAO, tmpObjectBlock.VertexCount,
				 tmpObjectBlock.VertexFormat, tmpObjectBlock.VertexArray.get(),
				 tmpObjectBlock.VertexStride * sizeof(tmpObjectBlock.VertexArray.get()[0]),
				 tmpObjectBlock.VBO, tmpObjectBlock.RangeStart,
				 tmpObjectBlock.IndexArray.get(), tmpObjectBlock.IBO))
			tmpObjectBlock.VAO = 0;
	}
}

/*
 * Recursively generate/calculate limited by size triangles.
 *
 * For result, we don't use tangent (second and third textures coordinates).
 * This is why we operate with Point[8], but not Point[12].
 * We use second and third textures coordinates for explosion shader in game code.
 */
static int RecursiveTrianglesLimitedBySize(float (&Point1)[8], float (&Point2)[8], float (&Point3)[8],
					   int Stride, float *VertexBuffer, int *CurrentPosition,
					   float TriangleSizeLimit)
{
	// calculate size for all triangle's side
	float Dist1 = sqrtf((Point1[0] - Point2[0]) * (Point1[0] - Point2[0]) +
			    (Point1[1] - Point2[1]) * (Point1[1] - Point2[1]) +
			    (Point1[2] - Point2[2]) * (Point1[2] - Point2[2]));
	float Dist2 = sqrtf((Point2[0] - Point3[0]) * (Point2[0] - Point3[0]) +
			    (Point2[1] - Point3[1]) * (Point2[1] - Point3[1]) +
			    (Point2[2] - Point3[2]) * (Point2[2] - Point3[2]));
	float Dist3 = sqrtf((Point3[0] - Point1[0]) * (Point3[0] - Point1[0]) +
			    (Point3[1] - Point1[1]) * (Point3[1] - Point1[1]) +
			    (Point3[2] - Point1[2]) * (Point3[2] - Point1[2]));

	if ((Dist1 <= TriangleSizeLimit) && (Dist2 <= TriangleSizeLimit) && (Dist3 <= TriangleSizeLimit)) {
		// nothing to do, leave
		if (VertexBuffer && CurrentPosition) {
			memcpy(VertexBuffer + (*CurrentPosition), Point1, sizeof(Point1[0]) * 8);
			*CurrentPosition += Stride;
			memcpy(VertexBuffer + (*CurrentPosition), Point2, sizeof(Point2[0]) * 8);
			*CurrentPosition += Stride;
			memcpy(VertexBuffer + (*CurrentPosition), Point3, sizeof(Point3[0]) * 8);
			*CurrentPosition += Stride;
		}
		return 1;
	}

	// find largest side and split it, so, we will have 2 triangles with smaller size
	if ((Dist1 > Dist2) && (Dist1 > Dist3)) {
		float Point_A[8]{(Point1[0] + Point2[0]) / 2.0f,
				 (Point1[1] + Point2[1]) / 2.0f,
				 (Point1[2] + Point2[2]) / 2.0f,
				 (Point1[3] + Point2[3]) / 2.0f,
				 (Point1[4] + Point2[4]) / 2.0f,
				 (Point1[5] + Point2[5]) / 2.0f,
				 (Point1[6] + Point2[6]) / 2.0f,
				 (Point1[7] + Point2[7]) / 2.0f};

		return RecursiveTrianglesLimitedBySize(Point1, Point_A, Point3, Stride, VertexBuffer,
						       CurrentPosition, TriangleSizeLimit) +
		       RecursiveTrianglesLimitedBySize(Point_A, Point2, Point3, Stride, VertexBuffer,
						       CurrentPosition, TriangleSizeLimit);
	} else if ((Dist2 > Dist1) && (Dist2 > Dist3)) {
		float Point_A[8]{(Point2[0] + Point3[0]) / 2.0f,
				 (Point2[1] + Point3[1]) / 2.0f,
				 (Point2[2] + Point3[2]) / 2.0f,
				 (Point2[3] + Point3[3]) / 2.0f,
				 (Point2[4] + Point3[4]) / 2.0f,
				 (Point2[5] + Point3[5]) / 2.0f,
				 (Point2[6] + Point3[6]) / 2.0f,
				 (Point2[7] + Point3[7]) / 2.0f};

		return RecursiveTrianglesLimitedBySize(Point1, Point2, Point_A, Stride, VertexBuffer,
						       CurrentPosition, TriangleSizeLimit) +
		       RecursiveTrianglesLimitedBySize(Point1, Point_A, Point3, Stride, VertexBuffer,
						       CurrentPosition, TriangleSizeLimit);
	} else {
		float Point_A[8]{(Point3[0] + Point1[0]) / 2.0f,
				 (Point3[1] + Point1[1]) / 2.0f,
				 (Point3[2] + Point1[2]) / 2.0f,
				 (Point3[3] + Point1[3]) / 2.0f,
				 (Point3[4] + Point1[4]) / 2.0f,
				 (Point3[5] + Point1[5]) / 2.0f,
				 (Point3[6] + Point1[6]) / 2.0f,
				 (Point3[7] + Point1[7]) / 2.0f};

		return RecursiveTrianglesLimitedBySize(Point1, Point2, Point_A, Stride, VertexBuffer,
						       CurrentPosition, TriangleSizeLimit) +
		       RecursiveTrianglesLimitedBySize(Point_A, Point2, Point3, Stride, VertexBuffer,
						       CurrentPosition, TriangleSizeLimit);
	}
}

/*
 * Create vertex array with limited by size triangles.
 * The idea is - split triangles recursively till they larger than size limit.
 *
 * For explosion we need pre-generated vertex array (VertexArrayWithSmallTriangles)
 * with small triangles, in this case, we could create cool looking effects, when
 * enemies disintegrate into 'dust' pieces during explosion.
 *
 * For result, we don't copy tangent (second and third textures coordinates).
 * This is why we operate with Point[8], but not Point[12].
 * We use second and third textures coordinates for explosion shader in game code, so,
 * we stay with VertexFormat and VertexStride and could point to object's VertexArray.
 */
static void CreateVertexArrayLimitedBySizeTriangles(sModel3D *Model, float TriangleSizeLimit)
{
	if (TriangleSizeLimit <= 0.0f) {
		for (auto &tmpObjectBlock : Model->ObjectBlocks) {
			tmpObjectBlock.VertexArrayWithSmallTrianglesCount = tmpObjectBlock.VertexCount;
			tmpObjectBlock.VertexArrayWithSmallTriangles = tmpObjectBlock.VertexArray;
		}
		return;
	}

	// calculate, how many memory we need for new vertex array
	for (auto &tmpObjectBlock : Model->ObjectBlocks) {
		tmpObjectBlock.VertexArrayWithSmallTrianglesCount = 0;

		// prepare 3 points (triangle)
		for (unsigned int j = 0; j < tmpObjectBlock.VertexCount; j += 3) {
			unsigned int tmpOffset0 = tmpObjectBlock.VertexStride * j;		// j
			unsigned int tmpOffset1 = tmpOffset0 + tmpObjectBlock.VertexStride;	// j + 1
			unsigned int tmpOffset2 = tmpOffset1 + tmpObjectBlock.VertexStride;	// j + 2

			float Point1[8]{tmpObjectBlock.VertexArray.get()[tmpOffset0],
					tmpObjectBlock.VertexArray.get()[tmpOffset0 + 1],
					tmpObjectBlock.VertexArray.get()[tmpOffset0 + 2],
					tmpObjectBlock.VertexArray.get()[tmpOffset0 + 3],
					tmpObjectBlock.VertexArray.get()[tmpOffset0 + 4],
					tmpObjectBlock.VertexArray.get()[tmpOffset0 + 5],
					tmpObjectBlock.VertexArray.get()[tmpOffset0 + 6],
					tmpObjectBlock.VertexArray.get()[tmpOffset0 + 7]};

			float Point2[8]{tmpObjectBlock.VertexArray.get()[tmpOffset1],
					tmpObjectBlock.VertexArray.get()[tmpOffset1 + 1],
					tmpObjectBlock.VertexArray.get()[tmpOffset1 + 2],
					tmpObjectBlock.VertexArray.get()[tmpOffset1 + 3],
					tmpObjectBlock.VertexArray.get()[tmpOffset1 + 4],
					tmpObjectBlock.VertexArray.get()[tmpOffset1 + 5],
					tmpObjectBlock.VertexArray.get()[tmpOffset1 + 6],
					tmpObjectBlock.VertexArray.get()[tmpOffset1 + 7]};

			float Point3[8]{tmpObjectBlock.VertexArray.get()[tmpOffset2],
					tmpObjectBlock.VertexArray.get()[tmpOffset2 + 1],
					tmpObjectBlock.VertexArray.get()[tmpOffset2 + 2],
					tmpObjectBlock.VertexArray.get()[tmpOffset2 + 3],
					tmpObjectBlock.VertexArray.get()[tmpOffset2 + 4],
					tmpObjectBlock.VertexArray.get()[tmpOffset2 + 5],
					tmpObjectBlock.VertexArray.get()[tmpOffset2 + 6],
					tmpObjectBlock.VertexArray.get()[tmpOffset2 + 7]};

			// recursively check, how many triangles we could receive
			tmpObjectBlock.VertexArrayWithSmallTrianglesCount +=
				RecursiveTrianglesLimitedBySize(Point1, Point2, Point3, 0,
								nullptr, nullptr, TriangleSizeLimit) * 3;
		}
	}

	// generate VertexArrayWithSmallTriangles
	for (auto &tmpObjectBlock : Model->ObjectBlocks) {
		// nothing to do
		if (tmpObjectBlock.VertexArrayWithSmallTrianglesCount == tmpObjectBlock.VertexCount)
			tmpObjectBlock.VertexArrayWithSmallTriangles = tmpObjectBlock.VertexArray;
		else {
			// allocate memory
			tmpObjectBlock.VertexArrayWithSmallTriangles.reset(
				new float[tmpObjectBlock.VertexArrayWithSmallTrianglesCount * tmpObjectBlock.VertexStride],
				std::default_delete<float[]>());
			int CurrentPosition = 0;

			// prepare 3 points (triangle)
			for (unsigned int j = 0; j < tmpObjectBlock.VertexCount; j += 3) {
				unsigned int tmpOffset0 = tmpObjectBlock.VertexStride * j;		// j
				unsigned int tmpOffset1 = tmpOffset0 + tmpObjectBlock.VertexStride;	// j + 1
				unsigned int tmpOffset2 = tmpOffset1 + tmpObjectBlock.VertexStride;	// j + 2

				float Point1[8]{tmpObjectBlock.VertexArray.get()[tmpOffset0],
						tmpObjectBlock.VertexArray.get()[tmpOffset0 + 1],
						tmpObjectBlock.VertexArray.get()[tmpOffset0 + 2],
						tmpObjectBlock.VertexArray.get()[tmpOffset0 + 3],
						tmpObjectBlock.VertexArray.get()[tmpOffset0 + 4],
						tmpObjectBlock.VertexArray.get()[tmpOffset0 + 5],
						tmpObjectBlock.VertexArray.get()[tmpOffset0 + 6],
						tmpObjectBlock.VertexArray.get()[tmpOffset0 + 7]};

				float Point2[8]{tmpObjectBlock.VertexArray.get()[tmpOffset1],
						tmpObjectBlock.VertexArray.get()[tmpOffset1 + 1],
						tmpObjectBlock.VertexArray.get()[tmpOffset1 + 2],
						tmpObjectBlock.VertexArray.get()[tmpOffset1 + 3],
						tmpObjectBlock.VertexArray.get()[tmpOffset1 + 4],
						tmpObjectBlock.VertexArray.get()[tmpOffset1 + 5],
						tmpObjectBlock.VertexArray.get()[tmpOffset1 + 6],
						tmpObjectBlock.VertexArray.get()[tmpOffset1 + 7]};

				float Point3[8]{tmpObjectBlock.VertexArray.get()[tmpOffset2],
						tmpObjectBlock.VertexArray.get()[tmpOffset2 + 1],
						tmpObjectBlock.VertexArray.get()[tmpOffset2 + 2],
						tmpObjectBlock.VertexArray.get()[tmpOffset2 + 3],
						tmpObjectBlock.VertexArray.get()[tmpOffset2 + 4],
						tmpObjectBlock.VertexArray.get()[tmpOffset2 + 5],
						tmpObjectBlock.VertexArray.get()[tmpOffset2 + 6],
						tmpObjectBlock.VertexArray.get()[tmpOffset2 + 7]};

				// recursively generate VertexArrayWithSmallTriangles
				RecursiveTrianglesLimitedBySize(Point1, Point2, Point3, tmpObjectBlock.VertexStride,
								tmpObjectBlock.VertexArrayWithSmallTriangles.get(),
								&CurrentPosition, TriangleSizeLimit);
			}
		}
	}
}

/*
 * Load 3D model.
 */
sModel3D *vw_LoadModel3D(const std::string &FileName, float TriangleSizeLimit, bool NeedTangentAndBinormal)
{
	if (FileName.empty())
		return nullptr;

	// if we already have it, just return a pointer.
	auto FoundModel = ModelsMap.find(FileName);
	if (FoundModel != ModelsMap.end())
		return &FoundModel->second;

	// check extension
	if (vw_CheckFileExtension(FileName, ".vw3d")) {
		if (!ModelsMap[FileName].LoadVW3D(FileName)) { // create ModelsMap entry on first access
			std::cout << "Can't load file ... " << FileName << "\n";
			ModelsMap.erase(FileName);
			return nullptr;
		}
	} else {
		std::cerr << __func__ << "(): " << "Format not supported " << FileName << "\n";
		return nullptr;
	}

	if (NeedTangentAndBinormal)
		CreateTangentAndBinormal(&ModelsMap[FileName]);
	CreateObjectsBuffers(&ModelsMap[FileName]);
	CreateHardwareBuffers(&ModelsMap[FileName]);
	CreateVertexArrayLimitedBySizeTriangles(&ModelsMap[FileName], TriangleSizeLimit);

	std::cout << "Loaded ... " << FileName << "\n";

	return &ModelsMap[FileName];
}

/*
 * Release all 3D models.
 */
void vw_ReleaseAllModel3D()
{
	ModelsMap.clear();
}

/*
 * Destructor sObjectBlock.
 */
sObjectBlock::~sObjectBlock()
{
	if (NeedDestroyDataInObjectBlock) {
		if (VBO)
			vw_DeleteBufferObject(VBO);
		if (IBO)
			vw_DeleteBufferObject(IBO);
		if (VAO)
			vw_DeleteVAO(VAO);
	}
}

/*
 * Destructor sModel3D.
 */
sModel3D::~sModel3D()
{
	if (!ObjectBlocks.empty()) {
		for (auto &tmpObjectBlock : ObjectBlocks) {
			if (tmpObjectBlock.VBO && (tmpObjectBlock.VBO != GlobalVBO))
				vw_DeleteBufferObject(tmpObjectBlock.VBO);
			if (tmpObjectBlock.IBO && (tmpObjectBlock.IBO != GlobalIBO))
				vw_DeleteBufferObject(tmpObjectBlock.IBO);
			if (tmpObjectBlock.VAO && (tmpObjectBlock.VAO != GlobalVAO))
				vw_DeleteVAO(tmpObjectBlock.VAO);
		}
	}
	if (GlobalVBO)
		vw_DeleteBufferObject(GlobalVBO);
	if (GlobalIBO)
		vw_DeleteBufferObject(GlobalIBO);
	if (GlobalVAO)
		vw_DeleteVAO(GlobalVAO);
}

/*
 * Load VW3D 3D models format.
 */
bool sModel3D::LoadVW3D(const std::string &FileName)
{
	if (FileName.empty())
		return false;

	std::unique_ptr<sFILE> File = vw_fopen(FileName);
	if (!File)
		return false;

	// check "VW3D" sign
	char Sign[4];
	File->fread(&Sign, 4, 1);
	// Sign don't contain null-terminated string, strncmp() should be used
	if (strncmp(Sign, "VW3D", 4) != 0)
		return false;

	std::uint32_t ObjectsListCount;
	File->fread(&ObjectsListCount, sizeof(ObjectsListCount), 1);

	ObjectBlocks.resize(ObjectsListCount);
	GlobalIndexArrayCount = 0;

	for (auto &tmpObjectBlock : ObjectBlocks) {
		tmpObjectBlock.RangeStart = GlobalIndexArrayCount;

		// VertexFormat
		File->fread(&(tmpObjectBlock.VertexFormat), sizeof(ObjectBlocks[0].VertexFormat), 1);
		// VertexStride
		File->fread(&(tmpObjectBlock.VertexStride), sizeof(ObjectBlocks[0].VertexStride), 1);
		// VertexCount
		File->fread(&(tmpObjectBlock.VertexCount), sizeof(ObjectBlocks[0].VertexCount), 1);
		GlobalIndexArrayCount += tmpObjectBlock.VertexCount;

		// Location
		File->fread(&(tmpObjectBlock.Location), sizeof(ObjectBlocks[0].Location.x) * 3, 1);
		// Rotation
		File->fread(&(tmpObjectBlock.Rotation), sizeof(ObjectBlocks[0].Rotation.x) * 3, 1);

		tmpObjectBlock.DrawType = ObjectDrawType::Normal;
		// vertex array-related
		tmpObjectBlock.NeedDestroyDataInObjectBlock = false;
		tmpObjectBlock.VertexArray = nullptr;
		tmpObjectBlock.VBO = 0;
		// index array-related
		tmpObjectBlock.IndexArray = nullptr;
		tmpObjectBlock.IBO = 0;
		// vao
		tmpObjectBlock.VAO = 0;
	}

	File->fread(&GlobalVertexArrayCount, sizeof(GlobalVertexArrayCount), 1);

	GlobalVertexArray.reset(new float[GlobalVertexArrayCount * ObjectBlocks[0].VertexStride],
				std::default_delete<float[]>());
	File->fread(GlobalVertexArray.get(),
		    GlobalVertexArrayCount * ObjectBlocks[0].VertexStride * sizeof(GlobalVertexArray.get()[0]),
		    1);

	// index array
	GlobalIndexArray.reset(new unsigned[GlobalIndexArrayCount], std::default_delete<unsigned[]>());
	File->fread(GlobalIndexArray.get(), GlobalIndexArrayCount * sizeof(GlobalIndexArray.get()[0]), 1);

	// setup points to global arrays
	for (auto &tmpObjectBlock : ObjectBlocks) {
		tmpObjectBlock.VertexArray = GlobalVertexArray;
		tmpObjectBlock.IndexArray = GlobalIndexArray;
	}

	vw_fclose(File);

	return true;
}

/*
 * Save VW3D 3D models format.
 */
bool sModel3D::SaveVW3D(const std::string &FileName)
{
	if (!GlobalVertexArray || !GlobalIndexArray || ObjectBlocks.empty()) {
		std::cerr << __func__ << "(): " << "Can't create " << FileName << " file for empty Model3D.\n";
		return false;
	}

	SDL_RWops *FileVW3D;
	FileVW3D = SDL_RWFromFile(FileName.c_str(), "wb");
	if (!FileVW3D) {
		std::cerr << __func__ << "(): " << "Can't create " << FileName << " file on disk.\n";
		return false;
	}

	// Sign for VW3D file format (magic number)
	constexpr char Sign[4]{'V','W','3','D'};
	SDL_RWwrite(FileVW3D, Sign, 4, 1);

	std::uint32_t ObjectsListCount = ObjectBlocks.size();
	SDL_RWwrite(FileVW3D, &ObjectsListCount, sizeof(ObjectsListCount), 1);

	for (auto &tmpObjectBlock : ObjectBlocks) {
		// VertexFormat
		SDL_RWwrite(FileVW3D, &tmpObjectBlock.VertexFormat, sizeof(ObjectBlocks[0].VertexFormat), 1);
		// VertexStride
		SDL_RWwrite(FileVW3D, &tmpObjectBlock.VertexStride, sizeof(ObjectBlocks[0].VertexStride), 1);
		// VertexCount
		SDL_RWwrite(FileVW3D, &tmpObjectBlock.VertexCount, sizeof(ObjectBlocks[0].VertexCount), 1);
		// Location
		SDL_RWwrite(FileVW3D, &tmpObjectBlock.Location, sizeof(ObjectBlocks[0].Location.x) * 3, 1);
		// Rotation
		SDL_RWwrite(FileVW3D, &tmpObjectBlock.Rotation, sizeof(ObjectBlocks[0].Rotation.x) * 3, 1);
	}

	SDL_RWwrite(FileVW3D, &GlobalVertexArrayCount, sizeof(GlobalVertexArrayCount), 1);
	SDL_RWwrite(FileVW3D, GlobalVertexArray.get(),
		    ObjectBlocks[0].VertexStride * GlobalVertexArrayCount * sizeof(GlobalVertexArray.get()[0]), 1);
	SDL_RWwrite(FileVW3D, GlobalIndexArray.get(), GlobalIndexArrayCount * sizeof(GlobalIndexArray.get()[0]), 1);
	SDL_RWclose(FileVW3D);

	std::cout << "VW3D Write: " << FileName << "\n";
	return true;
}
