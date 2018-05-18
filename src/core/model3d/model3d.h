/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
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
#include "../graphics/opengl.h"

struct sVECTOR3D;

using bounding_box = std::array<sVECTOR3D, 8>;

// Oriented Bounding Box, coordinates are related to object's center
struct sOBB {
	bounding_box Box{};
	sVECTOR3D Location{0.0f, 0.0f, 0.0f};
};

// Hit Bounding Box, coordinates are related to object's center
struct sHitBB {
	bounding_box Box{};
	sVECTOR3D Location{0.0f, 0.0f, 0.0f};
	float Radius2{0.0f}; // square of the radius of HitBB
	sVECTOR3D Size{0.0f, 0.0f, 0.0f}; // HitBB's size
};

enum class eObjectDrawType {
	Normal,
	Blend	// with blend (for planet's sky)
};

struct sObjectBlock {
	~sObjectBlock();

	// GLSL-related
	int ShaderType{1};
	std::array<float, 16> ShaderData{};

	sVECTOR3D Location{0.0f, 0.0f, 0.0f};
	sVECTOR3D Rotation{0.0f, 0.0f, 0.0f};

	// animation (rotation)
	bool NeedGeometryAnimation{false};
	sVECTOR3D GeometryAnimation{0.0f, 0.0f, 0.0f};

	// animation (tile animation)
	bool NeedTextureAnimation{false};
	sVECTOR3D TextureAnimation{0.0f, 0.0f, 0.0f};

	eObjectDrawType DrawType{eObjectDrawType::Normal};

	// initial index in global/local index array/indexbo, if we don't have
	// indexes at all, initial vertex in global/local vertex array/vertexbo
	unsigned int RangeStart{0};

	// if we allocate memory, bind vbo/ibo/vao personally for this object - release/delete this resources
	bool NeedDestroyDataInObjectBlock{false};

	// vertex quantity, that should be rendered
	unsigned int VertexQuantity{0};
	// vertex-related
	std::shared_ptr<float> VertexArray{}; // float[], make sure, that custom deleter are used
	int VertexFormat{0};
	unsigned int VertexStride{0}; // in bytes
	GLuint VBO{0};
	// index-related
	std::shared_ptr<unsigned> IndexArray{}; // unsigned[], make sure, that custom deleter are used
	GLuint IBO{0};
	// vao-related
	GLuint VAO{0};

	// for explosion we need pre-generated vertex array with small triangles,
	// in this case, we could create cool looking effects, when enemies disintegrate
	// into 'dust' pieces during explosion
	std::shared_ptr<float> VertexArrayWithSmallTriangles{}; // float[], make sure, that custom deleter are used
	unsigned int VertexArrayWithSmallTrianglesCount{0};
};

class cModel3D {
	friend std::weak_ptr<cModel3D> vw_LoadModel3D(const std::string &FileName, float TriangleSizeLimit,
						      bool NeedTangentAndBinormal);

public:
	// Load VW3D 3D models format.
	bool LoadVW3D(const std::string &FileName);
	// Save VW3D 3D models format.
	bool SaveVW3D(const std::string &FileName);

	// attached objects
	std::vector<sObjectBlock> ObjectBlocks{};

	// vertex-related
	std::shared_ptr<float> GlobalVertexArray{}; // float[], make sure, that custom deleter are used
	unsigned int GlobalVertexArrayCount{0}; // vertex quantity in GlobalVertexArray
	GLuint GlobalVBO{0};
	// index-related
	std::shared_ptr<unsigned> GlobalIndexArray{}; // unsigned[], make sure, that custom deleter are used
	unsigned int GlobalIndexArrayCount{0}; // GlobalIndexArray's size
	GLuint GlobalIBO{0};
	// vao-related
	GLuint GlobalVAO{0};

	// Axis-Aligned Bounding Box, coordinates are related to object's location
	bounding_box AABB{};
	// Oriented Bounding Box, coordinates are related to object's center
	sOBB OBB{};
	// Hit Bounding Box, same as OBB, but for each objects in the 3D model
	std::vector<sHitBB> HitBB{};
	// geometry center of all vertices in the model, related to object's center
	sVECTOR3D GeometryCenter{0.0f, 0.0f, 0.0f};
	float Radius{0.0f}; // Radius, for fast collisions check
	float Width{1.0f}; // calculated from 3D model actual size
	float Length{1.0f}; // calculated from 3D model actual size
	float Height{1.0f}; // calculated from 3D model actual size

private:
	// Don't allow direct new/delete usage in code, only vw_LoadModel3D()
	// allowed for Model3D creation and release setup (deleter must be provided).
	cModel3D() = default;
	~cModel3D();
};


// Load 3D model.
std::weak_ptr<cModel3D> vw_LoadModel3D(const std::string &FileName, float TriangleSizeLimit, bool NeedTangentAndBinormal);
// Release all 3D models.
void vw_ReleaseAllModel3D();

/*
 * 3D model's metadata initialization (AABB, OBB, HitBB, size, etc).
 */
template <typename T>
inline void vw_Model3DMetadataInitialization(T &Model)
{
	if (Model.ObjectBlocks.empty())
		return;

	int AllVertexCounted{0};

	// HitBB calculation
	Model.HitBB.resize(Model.ObjectBlocks.size());
	for (unsigned int i = 0; i < Model.ObjectBlocks.size(); i++) {
		float Matrix[9];
		vw_Matrix33CreateRotate(Matrix, Model.ObjectBlocks[i].Rotation);

		// first vertex's data as initial data for calculation
		int tmpOffset;
		if (Model.ObjectBlocks[i].IndexArray)
			tmpOffset = Model.ObjectBlocks[i].IndexArray.get()[Model.ObjectBlocks[i].RangeStart] *
				    Model.ObjectBlocks[i].VertexStride;
		else
			tmpOffset = Model.ObjectBlocks[i].RangeStart * Model.ObjectBlocks[i].VertexStride;

		sVECTOR3D tmpVertex;
		tmpVertex.x = Model.ObjectBlocks[i].VertexArray.get()[tmpOffset];
		tmpVertex.y = Model.ObjectBlocks[i].VertexArray.get()[tmpOffset + 1];
		tmpVertex.z = Model.ObjectBlocks[i].VertexArray.get()[tmpOffset + 2];
		vw_Matrix33CalcPoint(tmpVertex, Matrix);
		float MinX = tmpVertex.x;
		float MaxX = tmpVertex.x;
		float MinY = tmpVertex.y;
		float MaxY = tmpVertex.y;
		float MinZ = tmpVertex.z;
		float MaxZ = tmpVertex.z;

		Model.GeometryCenter += Model.ObjectBlocks[i].Location + tmpVertex;

		// starts from second vertex, check all vertices
		for (unsigned int j = 1; j < Model.ObjectBlocks[i].VertexQuantity; j++) {
			if (Model.ObjectBlocks[i].IndexArray)
				tmpOffset = Model.ObjectBlocks[i].IndexArray.get()[Model.ObjectBlocks[i].RangeStart + j] *
					    Model.ObjectBlocks[i].VertexStride;
			else
				tmpOffset = (Model.ObjectBlocks[i].RangeStart + j) * Model.ObjectBlocks[i].VertexStride;

			tmpVertex.x = Model.ObjectBlocks[i].VertexArray.get()[tmpOffset];
			tmpVertex.y = Model.ObjectBlocks[i].VertexArray.get()[tmpOffset + 1];
			tmpVertex.z = Model.ObjectBlocks[i].VertexArray.get()[tmpOffset + 2];
			vw_Matrix33CalcPoint(tmpVertex, Matrix);
			if (MinX > tmpVertex.x)
				MinX = tmpVertex.x;
			if (MinY > tmpVertex.y)
				MinY = tmpVertex.y;
			if (MinZ > tmpVertex.z)
				MinZ = tmpVertex.z;
			if (MaxX < tmpVertex.x)
				MaxX = tmpVertex.x;
			if (MaxY < tmpVertex.y)
				MaxY = tmpVertex.y;
			if (MaxZ < tmpVertex.z)
				MaxZ = tmpVertex.z;

			Model.GeometryCenter += Model.ObjectBlocks[i].Location + tmpVertex;
		}

		// store data to the HitBB
		Model.HitBB[i].Box[0] = sVECTOR3D(MaxX, MaxY, MaxZ);
		Model.HitBB[i].Box[1] = sVECTOR3D(MinX, MaxY, MaxZ);
		Model.HitBB[i].Box[2] = sVECTOR3D(MinX, MaxY, MinZ);
		Model.HitBB[i].Box[3] = sVECTOR3D(MaxX, MaxY, MinZ);
		Model.HitBB[i].Box[4] = sVECTOR3D(MaxX, MinY, MaxZ);
		Model.HitBB[i].Box[5] = sVECTOR3D(MinX, MinY, MaxZ);
		Model.HitBB[i].Box[6] = sVECTOR3D(MinX, MinY, MinZ);
		Model.HitBB[i].Box[7] = sVECTOR3D(MaxX, MinY, MinZ);

		// calculate HitBB geometry center
		Model.HitBB[i].Location.x = (MaxX + MinX) / 2.0f;
		Model.HitBB[i].Location.y = (MaxY + MinY) / 2.0f;
		Model.HitBB[i].Location.z = (MaxZ + MinZ) / 2.0f;

		// calculate HitBB size
		Model.HitBB[i].Size.x = fabsf(MaxX - MinX);
		Model.HitBB[i].Size.y = fabsf(MaxY - MinY);
		Model.HitBB[i].Size.z = fabsf(MaxZ - MinZ);

		// calculate HitBB radius square
		Model.HitBB[i].Radius2 = (Model.HitBB[i].Size.x / 2.0f) * (Model.HitBB[i].Size.x / 2.0f) +
					 (Model.HitBB[i].Size.y / 2.0f) * (Model.HitBB[i].Size.y / 2.0f) +
					 (Model.HitBB[i].Size.z / 2.0f) * (Model.HitBB[i].Size.z / 2.0f);

		// calculate HitBB points accordingly to HitBB geometry center location
		for (int k = 0; k < 8; k++) {
			Model.HitBB[i].Box[k] -= Model.HitBB[i].Location;
		}

		// correct HitBB geometry center location accordingly to object location
		Model.HitBB[i].Location += Model.ObjectBlocks[i].Location;

		AllVertexCounted += Model.ObjectBlocks[i].VertexQuantity;
	}

	// calculate 3D model's geometry center
	if (AllVertexCounted > 0) {
		// we are safe with static_cast here, since AllVertexCounted will not exceed limits
		Model.GeometryCenter = Model.GeometryCenter / static_cast<float>(AllVertexCounted);
	}

	// first HitBB's data as initial data for calculation
	float MinX = Model.HitBB[0].Box[6].x + Model.HitBB[0].Location.x;
	float MaxX = Model.HitBB[0].Box[0].x + Model.HitBB[0].Location.x;
	float MinY = Model.HitBB[0].Box[6].y + Model.HitBB[0].Location.y;
	float MaxY = Model.HitBB[0].Box[0].y + Model.HitBB[0].Location.y;
	float MinZ = Model.HitBB[0].Box[6].z + Model.HitBB[0].Location.z;
	float MaxZ = Model.HitBB[0].Box[0].z + Model.HitBB[0].Location.z;

	// starts from second HitBB, check all HitBBs
	for (unsigned int i = 1; i < Model.ObjectBlocks.size(); i++) {
		if (MinX > Model.HitBB[i].Box[6].x + Model.HitBB[i].Location.x)
			MinX = Model.HitBB[i].Box[6].x + Model.HitBB[i].Location.x;
		if (MaxX < Model.HitBB[i].Box[0].x + Model.HitBB[i].Location.x)
			MaxX = Model.HitBB[i].Box[0].x + Model.HitBB[i].Location.x;
		if (MinY > Model.HitBB[i].Box[6].y + Model.HitBB[i].Location.y)
			MinY = Model.HitBB[i].Box[6].y + Model.HitBB[i].Location.y;
		if (MaxY < Model.HitBB[i].Box[0].y + Model.HitBB[i].Location.y)
			MaxY = Model.HitBB[i].Box[0].y + Model.HitBB[i].Location.y;
		if (MinZ > Model.HitBB[i].Box[6].z + Model.HitBB[i].Location.z)
			MinZ = Model.HitBB[i].Box[6].z + Model.HitBB[i].Location.z;
		if (MaxZ < Model.HitBB[i].Box[0].z + Model.HitBB[i].Location.z)
			MaxZ = Model.HitBB[i].Box[0].z + Model.HitBB[i].Location.z;
	}

	// store data to OBB and AABB, since on this stage they are identical
	Model.OBB.Box[0] = Model.AABB[0] = sVECTOR3D(MaxX, MaxY, MaxZ);
	Model.OBB.Box[1] = Model.AABB[1] = sVECTOR3D(MinX, MaxY, MaxZ);
	Model.OBB.Box[2] = Model.AABB[2] = sVECTOR3D(MinX, MaxY, MinZ);
	Model.OBB.Box[3] = Model.AABB[3] = sVECTOR3D(MaxX, MaxY, MinZ);
	Model.OBB.Box[4] = Model.AABB[4] = sVECTOR3D(MaxX, MinY, MaxZ);
	Model.OBB.Box[5] = Model.AABB[5] = sVECTOR3D(MinX, MinY, MaxZ);
	Model.OBB.Box[6] = Model.AABB[6] = sVECTOR3D(MinX, MinY, MinZ);
	Model.OBB.Box[7] = Model.AABB[7] = sVECTOR3D(MaxX, MinY, MinZ);

	// calculate OBB geometry center
	Model.OBB.Location.x = (MaxX + MinX) / 2.0f;
	Model.OBB.Location.y = (MaxY + MinY) / 2.0f;
	Model.OBB.Location.z = (MaxZ + MinZ) / 2.0f;

	// calculate OBB points accordingly to OBB geometry center location
	for (int k = 0; k < 8; k++) {
		Model.OBB.Box[k] -= Model.OBB.Location;
	}

	// calculate 3D model's size
	Model.Width = fabsf(MaxX - MinX);
	Model.Height = fabsf(MaxY - MinY);
	Model.Length = fabsf(MaxZ - MinZ);

	// calculate 3D model's radius square
	float Width2 = Model.Width / 2.0f;
	float Length2 = Model.Length / 2.0f;
	float Height2 = Model.Height / 2.0f;
	Model.Radius = vw_sqrtf(Width2 * Width2 + Length2 * Length2 + Height2 * Height2);
}

#endif // MODEL3D_H
