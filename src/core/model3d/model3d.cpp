/****************************************************************************

    AstroMenace
    Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
    Copyright (c) 2006-2019 Mikhail Kurinnoi, Viewizard


    AstroMenace is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AstroMenace is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


    Website: https://viewizard.com/
    Project: https://github.com/viewizard/astromenace
    E-mail: viewizard@viewizard.com

*****************************************************************************/

#include "../graphics/graphics.h"
#include "../vfs/vfs.h"
#include "model3d.h"
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>

namespace viewizard {

class cModel3DWrapper : public sModel3D {
    friend std::weak_ptr<sModel3D> vw_LoadModel3D(const std::string &FileName, float TriangleSizeLimit,
                                                  bool NeedTangentAndBinormal);

public:
    // Load VW3D 3D models format.
    bool LoadVW3D(const std::string &FileName);
    // Save VW3D 3D models format.
    bool SaveVW3D(const std::string &FileName);

private:
    // Don't allow direct new/delete usage in code, only vw_LoadModel3D()
    // allowed for cModel3DWrapper creation and release setup (deleter must be provided).
    cModel3DWrapper() = default;
    ~cModel3DWrapper();
};

namespace {

// All loaded models.
std::unordered_map<std::string, std::shared_ptr<cModel3DWrapper>> ModelsMap;

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
static void CreateTangentAndBinormal(cModel3DWrapper *Model)
{
    int New_VertexFormat = RI_3f_XYZ | RI_3f_NORMAL | RI_3_TEX | RI_2f_TEX | RI_SEPARATE_TEX_COORD;
    int New_VertexStride = 3 + 3 + 6;

    // For vertex array with new size (we need 4 more floats for each vertex), copy
    // data with proper stride, we fill second and third textures coordinates later.
    // Forced to 'unpack' indexed arrays, since triangles with mutual points could
    // have different tangent data for same mutual point.
    if (Model->GlobalIndexArray) {
        std::shared_ptr<float> New_VertexBuffer{new float[New_VertexStride * Model->GlobalIndexArrayCount],
                                                std::default_delete<float[]>()};
        for (unsigned int j = 0; j < Model->GlobalIndexArrayCount; j++) {
            memcpy(New_VertexBuffer.get() + New_VertexStride * j,
                   Model->GlobalVertexArray.get() + Model->GlobalIndexArray.get()[j]
                                                    * Model->Chunks[0].VertexStride,
                   Model->Chunks[0].VertexStride * sizeof(float));
        }
        Model->GlobalVertexArray = New_VertexBuffer;
        Model->GlobalVertexArrayCount = Model->GlobalIndexArrayCount;
        for (auto &tmpChunk : Model->Chunks) {
            if (tmpChunk.IndexArray == Model->GlobalIndexArray) {
                tmpChunk.IndexArray.reset();
            }
        }
        Model->GlobalIndexArrayCount = 0;
        Model->GlobalIndexArray.reset();
    } else {
        std::shared_ptr<float> New_VertexBuffer{new float[New_VertexStride * Model->GlobalVertexArrayCount],
                std::default_delete<float[]>()};
        for (unsigned int j = 0; j < Model->GlobalVertexArrayCount; j++) {
            memcpy(New_VertexBuffer.get() + New_VertexStride * j,
                   Model->GlobalVertexArray.get() + j * Model->Chunks[0].VertexStride,
                   Model->Chunks[0].VertexStride * sizeof(float));
        }
        Model->GlobalVertexArray = New_VertexBuffer;
    }

    // calculate tangent and binormal
    for (unsigned int j = 0; j < Model->GlobalVertexArrayCount - 2; j += 3) {
        unsigned int tmpOffset0 = New_VertexStride * j;             // j
        unsigned int tmpOffset1 = tmpOffset0 + New_VertexStride;    // j + 1
        unsigned int tmpOffset2 = tmpOffset1 + New_VertexStride;    // j + 2

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

        float delta_U_0 = Model->GlobalVertexArray.get()[tmpOffset0 + 6]
                          - Model->GlobalVertexArray.get()[tmpOffset1 + 6];
        float delta_U_1 = Model->GlobalVertexArray.get()[tmpOffset2 + 6]
                          - Model->GlobalVertexArray.get()[tmpOffset1 + 6];

        float delta_V_0 = Model->GlobalVertexArray.get()[tmpOffset0 + 7]
                          - Model->GlobalVertexArray.get()[tmpOffset1 + 7];
        float delta_V_1 = Model->GlobalVertexArray.get()[tmpOffset2 + 7]
                          - Model->GlobalVertexArray.get()[tmpOffset1 + 7];

        // tangent and binormal
        sVECTOR3D Tangent = ((PlaneVector1 ^ delta_V_1) - (PlaneVector2 ^ delta_V_0));
        Tangent.NormalizeHi();
        float TangentW = 1.0f;
        sVECTOR3D BiNormal = ((PlaneVector1 ^ delta_U_1) - (PlaneVector2 ^ delta_U_0));
        BiNormal.NormalizeHi();

        // check, is normalmap mirrored or not
        sVECTOR3D TBCross = Tangent;
        TBCross.Multiply(BiNormal);
        if (TBCross * PlaneNormal < 0) {
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

    // store new vertex data
    for (auto &tmpChunk : Model->Chunks) {
        tmpChunk.VertexArray = Model->GlobalVertexArray;
        tmpChunk.VertexFormat = New_VertexFormat;
        tmpChunk.VertexStride = New_VertexStride;
    }
}

/*
 * Create vertex arrays for all chunks.
 */
static void CreateChunkBuffers(cModel3DWrapper *Model)
{
    for (auto &tmpChunk : Model->Chunks) {
        // vertex array
        tmpChunk.VertexArray.reset(new float[tmpChunk.VertexStride * tmpChunk.VertexQuantity],
                                   std::default_delete<float[]>());
        for (unsigned int j = 0; j < tmpChunk.VertexQuantity; j++) {
            unsigned int Offset{0};
            if (tmpChunk.IndexArray) {
                Offset = tmpChunk.IndexArray.get()[tmpChunk.RangeStart + j] * tmpChunk.VertexStride;
            } else {
                Offset = (tmpChunk.RangeStart + j) * tmpChunk.VertexStride;
            }
            memcpy(tmpChunk.VertexArray.get() + tmpChunk.VertexStride * j,
                   Model->GlobalVertexArray.get() + Offset,
                   tmpChunk.VertexStride * sizeof(tmpChunk.VertexArray.get()[0]));
        }

        // FIXME this code should be revised, we need index and vertex arrays with proper values here,
        //       for all models without binormals
        //       care about index and vertex counters for vbo/ibo in CreateHardwareBuffers()
        //       CreateVertexArrayLimitedBySizeTriangles() should be also revised, since we will be not able
        //       use chunk's vertex array 'as is' without unpacking

        // index array
        tmpChunk.IndexArray.reset();
        tmpChunk.RangeStart = 0;
    }
}

/*
 * Create all OpenGL-related hardware buffers.
 */
static void CreateHardwareBuffers(cModel3DWrapper *Model)
{
    // global vertex buffer object
    if (!vw_BuildBufferObject(eBufferObject::Vertex,
                              Model->GlobalVertexArrayCount * Model->Chunks[0].VertexStride * sizeof(float),
                              Model->GlobalVertexArray.get(), Model->GlobalVBO)) {
        Model->GlobalVBO = 0;
    }

    // global index buffer object
    if (!vw_BuildBufferObject(eBufferObject::Index, Model->GlobalIndexArrayCount * sizeof(unsigned),
                              Model->GlobalIndexArray.get(), Model->GlobalIBO)) {
        Model->GlobalIBO = 0;
    }

    // global vertex array object
    if (!vw_BuildVAO(Model->GlobalVAO, Model->Chunks[0].VertexFormat,
                     Model->Chunks[0].VertexStride * static_cast<GLsizei>(sizeof(float)),
                     Model->GlobalVBO, Model->GlobalIBO)) {
        Model->GlobalVAO = 0;
    }

    // and same for all chunks
    for (auto &tmpChunk : Model->Chunks) {
        // vertex buffer object
        if (!vw_BuildBufferObject(eBufferObject::Vertex,
                                  tmpChunk.VertexQuantity * tmpChunk.VertexStride * sizeof(float),
                                  tmpChunk.VertexArray.get(), tmpChunk.VBO)) {
            tmpChunk.VBO = 0;
        }

        // index buffer object
        if (!vw_BuildBufferObject(eBufferObject::Index, tmpChunk.VertexQuantity * sizeof(unsigned),
                                  tmpChunk.IndexArray.get(), tmpChunk.IBO)) {
            tmpChunk.IBO = 0;
        }

        // vertex array object
        if (!vw_BuildVAO(tmpChunk.VAO, tmpChunk.VertexFormat,
                         tmpChunk.VertexStride * static_cast<GLsizei>(sizeof(tmpChunk.VertexArray.get()[0])),
                         tmpChunk.VBO, tmpChunk.IBO)) {
            tmpChunk.VAO = 0;
        }
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
    float Dist1 = sqrtf((Point1[0] - Point2[0]) * (Point1[0] - Point2[0])
                        + (Point1[1] - Point2[1]) * (Point1[1] - Point2[1])
                        + (Point1[2] - Point2[2]) * (Point1[2] - Point2[2]));
    float Dist2 = sqrtf((Point2[0] - Point3[0]) * (Point2[0] - Point3[0])
                        + (Point2[1] - Point3[1]) * (Point2[1] - Point3[1])
                        + (Point2[2] - Point3[2]) * (Point2[2] - Point3[2]));
    float Dist3 = sqrtf((Point3[0] - Point1[0]) * (Point3[0] - Point1[0])
                        + (Point3[1] - Point1[1]) * (Point3[1] - Point1[1])
                        + (Point3[2] - Point1[2]) * (Point3[2] - Point1[2]));

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
    if (Dist1 > Dist2 && Dist1 > Dist3) {
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
    } else if (Dist2 > Dist1 && Dist2 > Dist3) {
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
 * we stay with VertexFormat and VertexStride and could point to chunk's VertexArray.
 */
static void CreateVertexArrayLimitedBySizeTriangles(cModel3DWrapper *Model, float TriangleSizeLimit)
{
    if (TriangleSizeLimit <= 0.0f) {
        for (auto &tmpChunk : Model->Chunks) {
            tmpChunk.VertexArrayWithSmallTrianglesCount = tmpChunk.VertexQuantity;
            tmpChunk.VertexArrayWithSmallTriangles = tmpChunk.VertexArray;
        }
        return;
    }

    // calculate, how many memory we need for new vertex array
    for (auto &tmpChunk : Model->Chunks) {
        tmpChunk.VertexArrayWithSmallTrianglesCount = 0;

        // prepare 3 points (triangle)
        for (unsigned int j = 0; j < tmpChunk.VertexQuantity; j += 3) {
            // CreateChunkBuffers() already 'unpacked' indexed arrays for Chunks,
            // so, we could use VertexArray directly here
            unsigned int tmpOffset0 = tmpChunk.VertexStride * j;            // j
            unsigned int tmpOffset1 = tmpOffset0 + tmpChunk.VertexStride;   // j + 1
            unsigned int tmpOffset2 = tmpOffset1 + tmpChunk.VertexStride;   // j + 2

            float Point1[8]{tmpChunk.VertexArray.get()[tmpOffset0],
                            tmpChunk.VertexArray.get()[tmpOffset0 + 1],
                            tmpChunk.VertexArray.get()[tmpOffset0 + 2],
                            tmpChunk.VertexArray.get()[tmpOffset0 + 3],
                            tmpChunk.VertexArray.get()[tmpOffset0 + 4],
                            tmpChunk.VertexArray.get()[tmpOffset0 + 5],
                            tmpChunk.VertexArray.get()[tmpOffset0 + 6],
                            tmpChunk.VertexArray.get()[tmpOffset0 + 7]};

            float Point2[8]{tmpChunk.VertexArray.get()[tmpOffset1],
                            tmpChunk.VertexArray.get()[tmpOffset1 + 1],
                            tmpChunk.VertexArray.get()[tmpOffset1 + 2],
                            tmpChunk.VertexArray.get()[tmpOffset1 + 3],
                            tmpChunk.VertexArray.get()[tmpOffset1 + 4],
                            tmpChunk.VertexArray.get()[tmpOffset1 + 5],
                            tmpChunk.VertexArray.get()[tmpOffset1 + 6],
                            tmpChunk.VertexArray.get()[tmpOffset1 + 7]};

            float Point3[8]{tmpChunk.VertexArray.get()[tmpOffset2],
                            tmpChunk.VertexArray.get()[tmpOffset2 + 1],
                            tmpChunk.VertexArray.get()[tmpOffset2 + 2],
                            tmpChunk.VertexArray.get()[tmpOffset2 + 3],
                            tmpChunk.VertexArray.get()[tmpOffset2 + 4],
                            tmpChunk.VertexArray.get()[tmpOffset2 + 5],
                            tmpChunk.VertexArray.get()[tmpOffset2 + 6],
                            tmpChunk.VertexArray.get()[tmpOffset2 + 7]};

            // recursively check, how many triangles we could receive
            tmpChunk.VertexArrayWithSmallTrianglesCount +=
                RecursiveTrianglesLimitedBySize(Point1, Point2, Point3, 0, nullptr, nullptr, TriangleSizeLimit) * 3;
        }
    }

    // generate VertexArrayWithSmallTriangles
    for (auto &tmpChunk : Model->Chunks) {
        // nothing to do
        if (tmpChunk.VertexArrayWithSmallTrianglesCount == tmpChunk.VertexQuantity) {
            tmpChunk.VertexArrayWithSmallTriangles = tmpChunk.VertexArray;
        } else {
            // allocate memory
            tmpChunk.VertexArrayWithSmallTriangles.reset(
                new float[tmpChunk.VertexArrayWithSmallTrianglesCount * tmpChunk.VertexStride],
                std::default_delete<float[]>());
            int CurrentPosition = 0;

            // prepare 3 points (triangle)
            for (unsigned int j = 0; j < tmpChunk.VertexQuantity; j += 3) {
                // CreateChunkBuffers() already 'unpacked' indexed arrays for Chunks,
                // so, we could use VertexArray directly here
                unsigned int tmpOffset0 = tmpChunk.VertexStride * j;            // j
                unsigned int tmpOffset1 = tmpOffset0 + tmpChunk.VertexStride;   // j + 1
                unsigned int tmpOffset2 = tmpOffset1 + tmpChunk.VertexStride;   // j + 2

                float Point1[8]{tmpChunk.VertexArray.get()[tmpOffset0],
                                tmpChunk.VertexArray.get()[tmpOffset0 + 1],
                                tmpChunk.VertexArray.get()[tmpOffset0 + 2],
                                tmpChunk.VertexArray.get()[tmpOffset0 + 3],
                                tmpChunk.VertexArray.get()[tmpOffset0 + 4],
                                tmpChunk.VertexArray.get()[tmpOffset0 + 5],
                                tmpChunk.VertexArray.get()[tmpOffset0 + 6],
                                tmpChunk.VertexArray.get()[tmpOffset0 + 7]};

                float Point2[8]{tmpChunk.VertexArray.get()[tmpOffset1],
                                tmpChunk.VertexArray.get()[tmpOffset1 + 1],
                                tmpChunk.VertexArray.get()[tmpOffset1 + 2],
                                tmpChunk.VertexArray.get()[tmpOffset1 + 3],
                                tmpChunk.VertexArray.get()[tmpOffset1 + 4],
                                tmpChunk.VertexArray.get()[tmpOffset1 + 5],
                                tmpChunk.VertexArray.get()[tmpOffset1 + 6],
                                tmpChunk.VertexArray.get()[tmpOffset1 + 7]};

                float Point3[8]{tmpChunk.VertexArray.get()[tmpOffset2],
                                tmpChunk.VertexArray.get()[tmpOffset2 + 1],
                                tmpChunk.VertexArray.get()[tmpOffset2 + 2],
                                tmpChunk.VertexArray.get()[tmpOffset2 + 3],
                                tmpChunk.VertexArray.get()[tmpOffset2 + 4],
                                tmpChunk.VertexArray.get()[tmpOffset2 + 5],
                                tmpChunk.VertexArray.get()[tmpOffset2 + 6],
                                tmpChunk.VertexArray.get()[tmpOffset2 + 7]};

                // recursively generate VertexArrayWithSmallTriangles
                RecursiveTrianglesLimitedBySize(Point1, Point2, Point3, tmpChunk.VertexStride,
                                                tmpChunk.VertexArrayWithSmallTriangles.get(),
                                                &CurrentPosition, TriangleSizeLimit);
            }
        }
    }
}

/*
 * Load 3D model.
 * Note, we don't provide shared_ptr, only weak_ptr, since all memory management
 * should be internal only. Caller should operate with weak_ptr and use lock()
 * (shared_ptr) only during access to model's data.
 * Note, FileName used as a key in ModelsMap, and should not be used with different
 * TriangleSizeLimit or NeedTangentAndBinormal.
 */
std::weak_ptr<sModel3D> vw_LoadModel3D(const std::string &FileName, float TriangleSizeLimit, bool NeedTangentAndBinormal)
{
    if (FileName.empty()) {
        return std::weak_ptr<sModel3D>{};
    }

    // if we already have it, just return previously loaded, we can't use same key twice
    auto FoundModel = ModelsMap.find(FileName);
    if (FoundModel != ModelsMap.end()) {
        return FoundModel->second;
    }

    ModelsMap.emplace(FileName, std::shared_ptr<cModel3DWrapper>{new cModel3DWrapper, [](cModel3DWrapper *p) {delete p;}});

    // check extension
    if (vw_CheckFileExtension(FileName, ".vw3d")) {
        if (!ModelsMap[FileName]->LoadVW3D(FileName)) {
            std::cout << "Can't load file ... " << FileName << "\n";
            ModelsMap.erase(FileName);
            return std::weak_ptr<sModel3D>{};
        }
    } else {
        std::cerr << __func__ << "(): " << "Format not supported " << FileName << "\n";
        ModelsMap.erase(FileName);
        return std::weak_ptr<sModel3D> {};
    }

    if (NeedTangentAndBinormal) {
        CreateTangentAndBinormal(ModelsMap[FileName].get());
    }
    CreateChunkBuffers(ModelsMap[FileName].get());
    CreateHardwareBuffers(ModelsMap[FileName].get());
    CreateVertexArrayLimitedBySizeTriangles(ModelsMap[FileName].get(), TriangleSizeLimit);

    std::cout << "Loaded ... " << FileName << "\n";

    return ModelsMap[FileName];
}

/*
 * Release all 3D models.
 */
void vw_ReleaseAllModel3D()
{
    ModelsMap.clear();
}

/*
 * Destructor sChunk3D.
 */
sChunk3D::~sChunk3D()
{
    if (NeedReleaseOpenGLBuffers) {
        if (VBO) {
            vw_DeleteBufferObject(VBO);
        }
        if (IBO) {
            vw_DeleteBufferObject(IBO);
        }
        if (VAO) {
            vw_DeleteVAO(VAO);
        }
    }
}

/*
 * Destructor cModel3DWrapper.
 */
cModel3DWrapper::~cModel3DWrapper()
{
    if (!Chunks.empty()) {
        for (auto &tmpChunk : Chunks) {
            if (tmpChunk.VBO && tmpChunk.VBO != GlobalVBO) {
                vw_DeleteBufferObject(tmpChunk.VBO);
            }
            if (tmpChunk.IBO && tmpChunk.IBO != GlobalIBO) {
                vw_DeleteBufferObject(tmpChunk.IBO);
            }
            if (tmpChunk.VAO && tmpChunk.VAO != GlobalVAO) {
                vw_DeleteVAO(tmpChunk.VAO);
            }
        }
    }
    if (GlobalVBO) {
        vw_DeleteBufferObject(GlobalVBO);
    }
    if (GlobalIBO) {
        vw_DeleteBufferObject(GlobalIBO);
    }
    if (GlobalVAO) {
        vw_DeleteVAO(GlobalVAO);
    }
}

/*
 * Load VW3D 3D models format.
 */
bool cModel3DWrapper::LoadVW3D(const std::string &FileName)
{
    if (FileName.empty()) {
        return false;
    }

    std::unique_ptr<cFILE> File = vw_fopen(FileName);
    if (!File) {
        return false;
    }

    // check "VW3D" sign
    char Sign[4];
    File->fread(&Sign, 4, 1);
    // Sign don't contain null-terminated string, strncmp() should be used
    if (strncmp(Sign, "VW3D", 4) != 0) {
        return false;
    }

    std::uint32_t ChunkArraySize;
    File->fread(&ChunkArraySize, sizeof(ChunkArraySize), 1);

    Chunks.resize(ChunkArraySize);
    GlobalIndexArrayCount = 0;

    for (auto &tmpChunk : Chunks) {
        tmpChunk.RangeStart = GlobalIndexArrayCount;

        // VertexFormat
        File->fread(&tmpChunk.VertexFormat, sizeof(Chunks[0].VertexFormat), 1);
        // VertexStride
        File->fread(&tmpChunk.VertexStride, sizeof(Chunks[0].VertexStride), 1);
        // VertexQuantity
        File->fread(&tmpChunk.VertexQuantity, sizeof(Chunks[0].VertexQuantity), 1);
        GlobalIndexArrayCount += tmpChunk.VertexQuantity;

        // Location
        File->fread(&tmpChunk.Location, sizeof(Chunks[0].Location.x) * 3, 1);
        // Rotation
        File->fread(&tmpChunk.Rotation, sizeof(Chunks[0].Rotation.x) * 3, 1);

        tmpChunk.DrawType = eModel3DDrawType::Normal;
        tmpChunk.NeedReleaseOpenGLBuffers = false;
        // vertex array-related
        tmpChunk.VBO = 0;
        // index array-related
        tmpChunk.IBO = 0;
        // vao-related
        tmpChunk.VAO = 0;

        assert(tmpChunk.VertexQuantity != 0);
    }

    File->fread(&GlobalVertexArrayCount, sizeof(GlobalVertexArrayCount), 1);

    GlobalVertexArray.reset(new float[GlobalVertexArrayCount * Chunks[0].VertexStride],
                            std::default_delete<float[]>());
    File->fread(GlobalVertexArray.get(),
                GlobalVertexArrayCount * Chunks[0].VertexStride * sizeof(GlobalVertexArray.get()[0]),
                1);

    // index array
    GlobalIndexArray.reset(new unsigned[GlobalIndexArrayCount], std::default_delete<unsigned[]>());
    File->fread(GlobalIndexArray.get(), GlobalIndexArrayCount * sizeof(GlobalIndexArray.get()[0]), 1);

    // setup points to global arrays
    for (auto &tmpChunk : Chunks) {
        tmpChunk.VertexArray = GlobalVertexArray;
        tmpChunk.IndexArray = GlobalIndexArray;
    }

    vw_fclose(File);

    MetadataInitialization();

    return true;
}

/*
 * Save VW3D 3D models format.
 */
bool cModel3DWrapper::SaveVW3D(const std::string &FileName)
{
    if (!GlobalVertexArray || !GlobalIndexArray || Chunks.empty()) {
        std::cerr << __func__ << "(): " << "Can't create " << FileName << " file for empty Model3D.\n";
        return false;
    }

    std::ofstream FileVW3D(FileName, std::ios::binary);
    if (FileVW3D.fail()) {
        std::cerr << __func__ << "(): " << "Can't create " << FileName << " file on disk.\n";
        return false;
    }

    // Sign for VW3D file format (magic number)
    constexpr char Sign[4]{'V','W','3','D'};
    FileVW3D.write(Sign, 4);

    std::uint32_t ChunkArraySize = static_cast<std::uint32_t>(Chunks.size());
    FileVW3D.write(reinterpret_cast<char*>(&ChunkArraySize), sizeof(ChunkArraySize));

    for (auto &tmpChunk : Chunks) {
        // VertexFormat
        FileVW3D.write(reinterpret_cast<char*>(&tmpChunk.VertexFormat),
                       sizeof(Chunks[0].VertexFormat));
        // VertexStride
        FileVW3D.write(reinterpret_cast<char*>(&tmpChunk.VertexStride),
                       sizeof(Chunks[0].VertexStride));
        // VertexQuantity
        FileVW3D.write(reinterpret_cast<char*>(&tmpChunk.VertexQuantity),
                       sizeof(Chunks[0].VertexQuantity));
        // Location
        FileVW3D.write(reinterpret_cast<char*>(&tmpChunk.Location),
                       sizeof(Chunks[0].Location.x) * 3);
        // Rotation
        FileVW3D.write(reinterpret_cast<char*>(&tmpChunk.Rotation),
                       sizeof(Chunks[0].Rotation.x) * 3);
    }

    FileVW3D.write(reinterpret_cast<char*>(&GlobalVertexArrayCount), sizeof(GlobalVertexArrayCount));
    FileVW3D.write(reinterpret_cast<char*>(GlobalVertexArray.get()),
                   Chunks[0].VertexStride * GlobalVertexArrayCount * sizeof(GlobalVertexArray.get()[0]));
    FileVW3D.write(reinterpret_cast<char*>(GlobalIndexArray.get()),
                   GlobalIndexArrayCount * sizeof(GlobalIndexArray.get()[0]));

    std::cout << "VW3D Write: " << FileName << "\n";
    return true;
}

/*
 * 3D model's metadata initialization (AABB, OBB, HitBB, size, etc).
 */
void sModel3D::MetadataInitialization()
{
    if (Chunks.empty()) {
        return;
    }

    unsigned int AllChunksVertexQuantity{0};

    // HitBB calculation
    HitBB.resize(Chunks.size());
    for (unsigned int i = 0; i < Chunks.size(); i++) {
        float Matrix[9];
        vw_Matrix33CreateRotate(Matrix, Chunks[i].Rotation);

        // first vertex's data as initial data for calculation
        int tmpOffset;
        if (Chunks[i].IndexArray) {
            tmpOffset = Chunks[i].IndexArray.get()[Chunks[i].RangeStart] * Chunks[i].VertexStride;
        } else {
            tmpOffset = Chunks[i].RangeStart * Chunks[i].VertexStride;
        }

        sVECTOR3D tmpVertex;
        tmpVertex.x = Chunks[i].VertexArray.get()[tmpOffset];
        tmpVertex.y = Chunks[i].VertexArray.get()[tmpOffset + 1];
        tmpVertex.z = Chunks[i].VertexArray.get()[tmpOffset + 2];
        vw_Matrix33CalcPoint(tmpVertex, Matrix);
        float MinX = tmpVertex.x;
        float MaxX = tmpVertex.x;
        float MinY = tmpVertex.y;
        float MaxY = tmpVertex.y;
        float MinZ = tmpVertex.z;
        float MaxZ = tmpVertex.z;

        GeometryCenter += Chunks[i].Location + tmpVertex;

        // starts from second vertex, check all vertices
        for (unsigned int j = 1; j < Chunks[i].VertexQuantity; j++) {
            if (Chunks[i].IndexArray) {
                tmpOffset = Chunks[i].IndexArray.get()[Chunks[i].RangeStart + j] * Chunks[i].VertexStride;
            } else {
                tmpOffset = (Chunks[i].RangeStart + j) * Chunks[i].VertexStride;
            }

            tmpVertex.x = Chunks[i].VertexArray.get()[tmpOffset];
            tmpVertex.y = Chunks[i].VertexArray.get()[tmpOffset + 1];
            tmpVertex.z = Chunks[i].VertexArray.get()[tmpOffset + 2];
            vw_Matrix33CalcPoint(tmpVertex, Matrix);
            if (MinX > tmpVertex.x) {
                MinX = tmpVertex.x;
            }
            if (MinY > tmpVertex.y) {
                MinY = tmpVertex.y;
            }
            if (MinZ > tmpVertex.z) {
                MinZ = tmpVertex.z;
            }
            if (MaxX < tmpVertex.x) {
                MaxX = tmpVertex.x;
            }
            if (MaxY < tmpVertex.y) {
                MaxY = tmpVertex.y;
            }
            if (MaxZ < tmpVertex.z) {
                MaxZ = tmpVertex.z;
            }

            GeometryCenter += Chunks[i].Location + tmpVertex;
        }

        // store data to the HitBB
        HitBB[i].Box[0] = sVECTOR3D{MaxX, MaxY, MaxZ};
        HitBB[i].Box[1] = sVECTOR3D{MinX, MaxY, MaxZ};
        HitBB[i].Box[2] = sVECTOR3D{MinX, MaxY, MinZ};
        HitBB[i].Box[3] = sVECTOR3D{MaxX, MaxY, MinZ};
        HitBB[i].Box[4] = sVECTOR3D{MaxX, MinY, MaxZ};
        HitBB[i].Box[5] = sVECTOR3D{MinX, MinY, MaxZ};
        HitBB[i].Box[6] = sVECTOR3D{MinX, MinY, MinZ};
        HitBB[i].Box[7] = sVECTOR3D{MaxX, MinY, MinZ};

        // calculate HitBB geometry center
        HitBB[i].Location.x = (MaxX + MinX) / 2.0f;
        HitBB[i].Location.y = (MaxY + MinY) / 2.0f;
        HitBB[i].Location.z = (MaxZ + MinZ) / 2.0f;

        // calculate HitBB size
        HitBB[i].Size.x = fabsf(MaxX - MinX);
        HitBB[i].Size.y = fabsf(MaxY - MinY);
        HitBB[i].Size.z = fabsf(MaxZ - MinZ);

        // calculate HitBB radius square
        HitBB[i].Radius2 = (HitBB[i].Size.x / 2.0f) * (HitBB[i].Size.x / 2.0f)
                           + (HitBB[i].Size.y / 2.0f) * (HitBB[i].Size.y / 2.0f)
                           + (HitBB[i].Size.z / 2.0f) * (HitBB[i].Size.z / 2.0f);

        // calculate HitBB points accordingly to HitBB geometry center location
        for (int k = 0; k < 8; k++) {
            HitBB[i].Box[k] -= HitBB[i].Location;
        }

        // correct HitBB geometry center location accordingly to chunk location
        HitBB[i].Location += Chunks[i].Location;

        AllChunksVertexQuantity += Chunks[i].VertexQuantity;
    }

    // calculate 3D model's geometry center
    if (AllChunksVertexQuantity > 0) {
        // we are safe with static_cast here, since AllChunksVertexQuantity will not exceed limits
        GeometryCenter = GeometryCenter / static_cast<float>(AllChunksVertexQuantity);
    }

    // first HitBB's data as initial data for calculation
    float MinX = HitBB[0].Box[6].x + HitBB[0].Location.x;
    float MaxX = HitBB[0].Box[0].x + HitBB[0].Location.x;
    float MinY = HitBB[0].Box[6].y + HitBB[0].Location.y;
    float MaxY = HitBB[0].Box[0].y + HitBB[0].Location.y;
    float MinZ = HitBB[0].Box[6].z + HitBB[0].Location.z;
    float MaxZ = HitBB[0].Box[0].z + HitBB[0].Location.z;

    // starts from second HitBB, check all HitBBs
    for (unsigned int i = 1; i < Chunks.size(); i++) {
        if (MinX > HitBB[i].Box[6].x + HitBB[i].Location.x) {
            MinX = HitBB[i].Box[6].x + HitBB[i].Location.x;
        }
        if (MaxX < HitBB[i].Box[0].x + HitBB[i].Location.x) {
            MaxX = HitBB[i].Box[0].x + HitBB[i].Location.x;
        }
        if (MinY > HitBB[i].Box[6].y + HitBB[i].Location.y) {
            MinY = HitBB[i].Box[6].y + HitBB[i].Location.y;
        }
        if (MaxY < HitBB[i].Box[0].y + HitBB[i].Location.y) {
            MaxY = HitBB[i].Box[0].y + HitBB[i].Location.y;
        }
        if (MinZ > HitBB[i].Box[6].z + HitBB[i].Location.z) {
            MinZ = HitBB[i].Box[6].z + HitBB[i].Location.z;
        }
        if (MaxZ < HitBB[i].Box[0].z + HitBB[i].Location.z) {
            MaxZ = HitBB[i].Box[0].z + HitBB[i].Location.z;
        }
    }

    // store data to OBB and AABB, since on this stage they are identical
    OBB.Box[0] = AABB[0] = sVECTOR3D{MaxX, MaxY, MaxZ};
    OBB.Box[1] = AABB[1] = sVECTOR3D{MinX, MaxY, MaxZ};
    OBB.Box[2] = AABB[2] = sVECTOR3D{MinX, MaxY, MinZ};
    OBB.Box[3] = AABB[3] = sVECTOR3D{MaxX, MaxY, MinZ};
    OBB.Box[4] = AABB[4] = sVECTOR3D{MaxX, MinY, MaxZ};
    OBB.Box[5] = AABB[5] = sVECTOR3D{MinX, MinY, MaxZ};
    OBB.Box[6] = AABB[6] = sVECTOR3D{MinX, MinY, MinZ};
    OBB.Box[7] = AABB[7] = sVECTOR3D{MaxX, MinY, MinZ};

    // calculate OBB geometry center
    OBB.Location.x = (MaxX + MinX) / 2.0f;
    OBB.Location.y = (MaxY + MinY) / 2.0f;
    OBB.Location.z = (MaxZ + MinZ) / 2.0f;

    // calculate OBB points accordingly to OBB geometry center location
    for (int k = 0; k < 8; k++) {
        OBB.Box[k] -= OBB.Location;
    }

    // calculate 3D model's size
    Width = fabsf(MaxX - MinX);
    Height = fabsf(MaxY - MinY);
    Length = fabsf(MaxZ - MinZ);

    // calculate 3D model's radius square
    float Width2 = Width / 2.0f;
    float Length2 = Length / 2.0f;
    float Height2 = Height / 2.0f;
    Radius = vw_sqrtf(Width2 * Width2 + Length2 * Length2 + Height2 * Height2);
}

} // viewizard namespace
