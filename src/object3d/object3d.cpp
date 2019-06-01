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

// TODO provide more rendering optimization in DrawObjectStatus()

// TODO codestyle should be fixed

#include "object3d.h"
#include "../config/config.h"
#include "../gfx/shadow_map.h"
#include "../script/script.h"
#include "../game.h" // FIXME "game.h" should be replaced by individual headers

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

eRenderBoundingBoxes BBRenderMode{eRenderBoundingBoxes::None};

} // unnamed namespace

// FIXME should be fixed, don't allow global scope interaction for local variables
extern std::weak_ptr<cGLSL> GLSLShaderType1;
extern std::weak_ptr<cGLSL> GLSLShaderType2;
extern std::weak_ptr<cGLSL> GLSLShaderType3;


/*
 * Set chunk location.
 */
void cObject3D::SetChunkLocation(const sVECTOR3D &NewLocation, unsigned ChunkNum)
{
    if (!HitBB.empty()) {
        float OldInvRotationMatTmp[9];
        memcpy(OldInvRotationMatTmp, CurrentRotationMat, 9 * sizeof(CurrentRotationMat[0]));
        vw_Matrix33InverseRotate(OldInvRotationMatTmp);

        vw_Matrix33CalcPoint(HitBB[ChunkNum].Location, OldInvRotationMatTmp);
        HitBB[ChunkNum].Location -= Chunks[ChunkNum].Location - NewLocation;
        vw_Matrix33CalcPoint(HitBB[ChunkNum].Location, CurrentRotationMat);

        float MinX = 10000.0f;
        float MaxX = -10000.0f;
        float MinY = 10000.0f;
        float MaxY = -10000.0f;
        float MinZ = 10000.0f;
        float MaxZ = -10000.0f;

        for (unsigned int i = 0; i < Chunks.size(); i++) {
            vw_Matrix33CalcPoint(HitBB[i].Location, OldInvRotationMatTmp);

            for (int j = 0; j < 8; j++) {
                vw_Matrix33CalcPoint(HitBB[i].Box[j], OldInvRotationMatTmp);

                if (MinX > HitBB[i].Box[j].x + HitBB[i].Location.x) {
                    MinX = HitBB[i].Box[j].x + HitBB[i].Location.x;
                }
                if (MaxX < HitBB[i].Box[j].x + HitBB[i].Location.x) {
                    MaxX = HitBB[i].Box[j].x + HitBB[i].Location.x;
                }
                if (MinY > HitBB[i].Box[j].y + HitBB[i].Location.y) {
                    MinY = HitBB[i].Box[j].y + HitBB[i].Location.y;
                }
                if (MaxY < HitBB[i].Box[j].y + HitBB[i].Location.y) {
                    MaxY = HitBB[i].Box[j].y + HitBB[i].Location.y;
                }
                if (MinZ > HitBB[i].Box[j].z + HitBB[i].Location.z) {
                    MinZ = HitBB[i].Box[j].z + HitBB[i].Location.z;
                }
                if (MaxZ < HitBB[i].Box[j].z + HitBB[i].Location.z) {
                    MaxZ = HitBB[i].Box[j].z + HitBB[i].Location.z;
                }

                vw_Matrix33CalcPoint(HitBB[i].Box[j], CurrentRotationMat);
            }
            vw_Matrix33CalcPoint(HitBB[i].Location, CurrentRotationMat);
        }

        OBB.Box[0] = sVECTOR3D{MaxX, MaxY, MaxZ};
        OBB.Box[1] = sVECTOR3D{MinX, MaxY, MaxZ};
        OBB.Box[2] = sVECTOR3D{MinX, MaxY, MinZ};
        OBB.Box[3] = sVECTOR3D{MaxX, MaxY, MinZ};
        OBB.Box[4] = sVECTOR3D{MaxX, MinY, MaxZ};
        OBB.Box[5] = sVECTOR3D{MinX, MinY, MaxZ};
        OBB.Box[6] = sVECTOR3D{MinX, MinY, MinZ};
        OBB.Box[7] = sVECTOR3D{MaxX, MinY, MinZ};

        Width = fabsf(MaxX - MinX);
        Height = fabsf(MaxY - MinY);
        Length = fabsf(MaxZ - MinZ);

        float Width2 = Width / 2.0f;
        float Length2 = Length / 2.0f;
        float Height2 = Height / 2.0f;
        Radius = vw_sqrtf(Width2 * Width2 + Length2 * Length2 + Height2 * Height2);

        OBB.Location.x = (MaxX + MinX) / 2.0f;
        OBB.Location.y = (MaxY + MinY) / 2.0f;
        OBB.Location.z = (MaxZ + MinZ) / 2.0f;

        for (int k = 0; k < 8; k++) {
            OBB.Box[k] -= OBB.Location;
            vw_Matrix33CalcPoint(OBB.Box[k], CurrentRotationMat);
        }
        vw_Matrix33CalcPoint(OBB.Location, CurrentRotationMat);

        MinX = MaxX = OBB.Box[0].x + OBB.Location.x;
        MinY = MaxY = OBB.Box[0].y + OBB.Location.y;
        MinZ = MaxZ = OBB.Box[0].z + OBB.Location.z;
        for (int j = 0; j < 8; j++) {
            if (MinX > OBB.Box[j].x + OBB.Location.x) {
                MinX = OBB.Box[j].x + OBB.Location.x;
            }
            if (MinY > OBB.Box[j].y + OBB.Location.y) {
                MinY = OBB.Box[j].y + OBB.Location.y;
            }
            if (MinZ > OBB.Box[j].z + OBB.Location.z) {
                MinZ = OBB.Box[j].z + OBB.Location.z;
            }
            if (MaxX < OBB.Box[j].x + OBB.Location.x) {
                MaxX = OBB.Box[j].x + OBB.Location.x;
            }
            if (MaxY < OBB.Box[j].y + OBB.Location.y) {
                MaxY = OBB.Box[j].y + OBB.Location.y;
            }
            if (MaxZ < OBB.Box[j].z + OBB.Location.z) {
                MaxZ = OBB.Box[j].z + OBB.Location.z;
            }
        }
        AABB[0] = sVECTOR3D{MaxX, MaxY, MaxZ};
        AABB[1] = sVECTOR3D{MinX, MaxY, MaxZ};
        AABB[2] = sVECTOR3D{MinX, MaxY, MinZ};
        AABB[3] = sVECTOR3D{MaxX, MaxY, MinZ};
        AABB[4] = sVECTOR3D{MaxX, MinY, MaxZ};
        AABB[5] = sVECTOR3D{MinX, MinY, MaxZ};
        AABB[6] = sVECTOR3D{MinX, MinY, MinZ};
        AABB[7] = sVECTOR3D{MaxX, MinY, MinZ};
    }

    Chunks[ChunkNum].Location = NewLocation;
}

/*
 * Set chunk rotation.
 */
void cObject3D::SetChunkRotation(const sVECTOR3D &NewRotation, unsigned ChunkNum)
{
    if (!HitBB.empty()) {
        float CurrentRotationMatTmp2[9];
        vw_Matrix33CreateRotate(CurrentRotationMatTmp2, NewRotation);

        float OldInvRotationMatTmp2[9];
        vw_Matrix33CreateRotate(OldInvRotationMatTmp2, Chunks[ChunkNum].Rotation);
        vw_Matrix33InverseRotate(OldInvRotationMatTmp2);

        float OldInvRotationMatTmp[9];
        memcpy(OldInvRotationMatTmp, CurrentRotationMat, 9 * sizeof(CurrentRotationMat[0]));
        vw_Matrix33InverseRotate(OldInvRotationMatTmp);

        vw_Matrix33CalcPoint(HitBB[ChunkNum].Location, OldInvRotationMatTmp);
        HitBB[ChunkNum].Location -= Chunks[ChunkNum].Location;
        vw_Matrix33CalcPoint(HitBB[ChunkNum].Location, OldInvRotationMatTmp2);
        vw_Matrix33CalcPoint(HitBB[ChunkNum].Location, CurrentRotationMatTmp2);
        HitBB[ChunkNum].Location += Chunks[ChunkNum].Location;
        vw_Matrix33CalcPoint(HitBB[ChunkNum].Location, CurrentRotationMat);
        for (int j = 0; j < 8; j++) {
            vw_Matrix33CalcPoint(HitBB[ChunkNum].Box[j], OldInvRotationMatTmp);
            vw_Matrix33CalcPoint(HitBB[ChunkNum].Box[j], OldInvRotationMatTmp2);
            vw_Matrix33CalcPoint(HitBB[ChunkNum].Box[j], CurrentRotationMatTmp2);
            vw_Matrix33CalcPoint(HitBB[ChunkNum].Box[j], CurrentRotationMat);
        }

        float MinX = 10000.0f;
        float MaxX = -10000.0f;
        float MinY = 10000.0f;
        float MaxY = -10000.0f;
        float MinZ = 10000.0f;
        float MaxZ = -10000.0f;

        for (unsigned int i = 0; i < Chunks.size(); i++) {
            vw_Matrix33CalcPoint(HitBB[i].Location, OldInvRotationMatTmp);

            for (int j = 0; j < 8; j++) {
                vw_Matrix33CalcPoint(HitBB[i].Box[j], OldInvRotationMatTmp);

                if (MinX > HitBB[i].Box[j].x + HitBB[i].Location.x) {
                    MinX = HitBB[i].Box[j].x + HitBB[i].Location.x;
                }
                if (MaxX < HitBB[i].Box[j].x + HitBB[i].Location.x) {
                    MaxX = HitBB[i].Box[j].x + HitBB[i].Location.x;
                }
                if (MinY > HitBB[i].Box[j].y + HitBB[i].Location.y) {
                    MinY = HitBB[i].Box[j].y + HitBB[i].Location.y;
                }
                if (MaxY < HitBB[i].Box[j].y + HitBB[i].Location.y) {
                    MaxY = HitBB[i].Box[j].y + HitBB[i].Location.y;
                }
                if (MinZ > HitBB[i].Box[j].z + HitBB[i].Location.z) {
                    MinZ = HitBB[i].Box[j].z + HitBB[i].Location.z;
                }
                if (MaxZ < HitBB[i].Box[j].z + HitBB[i].Location.z) {
                    MaxZ = HitBB[i].Box[j].z + HitBB[i].Location.z;
                }

                vw_Matrix33CalcPoint(HitBB[i].Box[j], CurrentRotationMat);
            }
            vw_Matrix33CalcPoint(HitBB[i].Location, CurrentRotationMat);
        }

        OBB.Box[0] = sVECTOR3D{MaxX, MaxY, MaxZ};
        OBB.Box[1] = sVECTOR3D{MinX, MaxY, MaxZ};
        OBB.Box[2] = sVECTOR3D{MinX, MaxY, MinZ};
        OBB.Box[3] = sVECTOR3D{MaxX, MaxY, MinZ};
        OBB.Box[4] = sVECTOR3D{MaxX, MinY, MaxZ};
        OBB.Box[5] = sVECTOR3D{MinX, MinY, MaxZ};
        OBB.Box[6] = sVECTOR3D{MinX, MinY, MinZ};
        OBB.Box[7] = sVECTOR3D{MaxX, MinY, MinZ};

        Width = fabsf(MaxX - MinX);
        Height = fabsf(MaxY - MinY);
        Length = fabsf(MaxZ - MinZ);

        float Width2 = Width / 2.0f;
        float Length2 = Length / 2.0f;
        float Height2 = Height / 2.0f;
        Radius = vw_sqrtf(Width2 * Width2 + Length2 * Length2 + Height2 * Height2);

        OBB.Location.x = (MaxX + MinX) / 2.0f;
        OBB.Location.y = (MaxY + MinY) / 2.0f;
        OBB.Location.z = (MaxZ + MinZ) / 2.0f;

        for (int k = 0; k < 8; k++) {
            OBB.Box[k] -= OBB.Location;
            vw_Matrix33CalcPoint(OBB.Box[k], CurrentRotationMat);
        }
        vw_Matrix33CalcPoint(OBB.Location, CurrentRotationMat);

        MinX = MaxX = OBB.Box[0].x + OBB.Location.x;
        MinY = MaxY = OBB.Box[0].y + OBB.Location.y;
        MinZ = MaxZ = OBB.Box[0].z + OBB.Location.z;
        for (int j = 0; j < 8; j++) {
            if (MinX > OBB.Box[j].x + OBB.Location.x) {
                MinX = OBB.Box[j].x + OBB.Location.x;
            }
            if (MinY > OBB.Box[j].y + OBB.Location.y) {
                MinY = OBB.Box[j].y + OBB.Location.y;
            }
            if (MinZ > OBB.Box[j].z + OBB.Location.z) {
                MinZ = OBB.Box[j].z + OBB.Location.z;
            }
            if (MaxX < OBB.Box[j].x + OBB.Location.x) {
                MaxX = OBB.Box[j].x + OBB.Location.x;
            }
            if (MaxY < OBB.Box[j].y + OBB.Location.y) {
                MaxY = OBB.Box[j].y + OBB.Location.y;
            }
            if (MaxZ < OBB.Box[j].z + OBB.Location.z) {
                MaxZ = OBB.Box[j].z + OBB.Location.z;
            }
        }
        AABB[0] = sVECTOR3D{MaxX, MaxY, MaxZ};
        AABB[1] = sVECTOR3D{MinX, MaxY, MaxZ};
        AABB[2] = sVECTOR3D{MinX, MaxY, MinZ};
        AABB[3] = sVECTOR3D{MaxX, MaxY, MinZ};
        AABB[4] = sVECTOR3D{MaxX, MinY, MaxZ};
        AABB[5] = sVECTOR3D{MinX, MinY, MaxZ};
        AABB[6] = sVECTOR3D{MinX, MinY, MinZ};
        AABB[7] = sVECTOR3D{MaxX, MinY, MinZ};
    }

    Chunks[ChunkNum].Rotation = NewRotation;
}

/*
 * Set location.
 */
void cObject3D::SetLocation(const sVECTOR3D &NewLocation)
{
    PrevLocation = Location;
    Location = NewLocation;
}

/*
 * Set rotation.
 */
void cObject3D::SetRotation(const sVECTOR3D &NewRotation)
{
    OldRotationInv = sVECTOR3D{-Rotation.x, -Rotation.y, -Rotation.z};
    Rotation += NewRotation;

    memcpy(OldInvRotationMat, CurrentRotationMat, 9 * sizeof(CurrentRotationMat[0]));
    vw_Matrix33InverseRotate(OldInvRotationMat);
    vw_Matrix33CreateRotate(CurrentRotationMat, Rotation);

    vw_Matrix33CalcPoint(Orientation, OldInvRotationMat);
    vw_Matrix33CalcPoint(Orientation, CurrentRotationMat);

    if (!HitBB.empty()) {
        for (unsigned int i = 0; i < Chunks.size(); i++) {
            vw_Matrix33CalcPoint(HitBB[i].Location, OldInvRotationMat);
            vw_Matrix33CalcPoint(HitBB[i].Location, CurrentRotationMat);

            for (int j = 0; j < 8; j++) {
                vw_Matrix33CalcPoint(HitBB[i].Box[j], OldInvRotationMat);
                vw_Matrix33CalcPoint(HitBB[i].Box[j], CurrentRotationMat);
            }
        }
    }

    vw_Matrix33CalcPoint(OBB.Location, OldInvRotationMat);
    vw_Matrix33CalcPoint(OBB.Location, CurrentRotationMat);
    for (int j = 0; j < 8; j++) {
        vw_Matrix33CalcPoint(OBB.Box[j], OldInvRotationMat);
        vw_Matrix33CalcPoint(OBB.Box[j], CurrentRotationMat);
    }

    float MinX = 10000.0f;
    float MaxX = -10000.0f;
    float MinY = 10000.0f;
    float MaxY = -10000.0f;
    float MinZ = 10000.0f;
    float MaxZ = -10000.0f;
    for (int j = 0; j < 8; j++) {
        if (MinX > OBB.Box[j].x + OBB.Location.x) {
            MinX = OBB.Box[j].x + OBB.Location.x;
        }
        if (MinY > OBB.Box[j].y + OBB.Location.y) {
            MinY = OBB.Box[j].y + OBB.Location.y;
        }
        if (MinZ > OBB.Box[j].z + OBB.Location.z) {
            MinZ = OBB.Box[j].z + OBB.Location.z;
        }
        if (MaxX < OBB.Box[j].x + OBB.Location.x) {
            MaxX = OBB.Box[j].x + OBB.Location.x;
        }
        if (MaxY < OBB.Box[j].y + OBB.Location.y) {
            MaxY = OBB.Box[j].y + OBB.Location.y;
        }
        if (MaxZ < OBB.Box[j].z + OBB.Location.z) {
            MaxZ = OBB.Box[j].z + OBB.Location.z;
        }
    }
    AABB[0] = sVECTOR3D{MaxX, MaxY, MaxZ};
    AABB[1] = sVECTOR3D{MinX, MaxY, MaxZ};
    AABB[2] = sVECTOR3D{MinX, MaxY, MinZ};
    AABB[3] = sVECTOR3D{MaxX, MaxY, MinZ};
    AABB[4] = sVECTOR3D{MaxX, MinY, MaxZ};
    AABB[5] = sVECTOR3D{MinX, MinY, MaxZ};
    AABB[6] = sVECTOR3D{MinX, MinY, MinZ};
    AABB[7] = sVECTOR3D{MaxX, MinY, MinZ};

}

/*
 * Draw line.
 * Since this is debug only code, no need in future rendering optimization (vbo/ibo/vao, etc...).
 */
static void DrawLine(const sVECTOR3D &Point1, const sVECTOR3D &Point2, const sRGBCOLOR &Color)
{
    constexpr float Alpha{1.0f};

    // 2 points for line with RI_3f_XYZ | RI_4f_COLOR = 2 * (3 + 4) = 14
    float DrawArray[14]{Point1.x, Point1.y, Point1.z, Color.r, Color.g, Color.b, Alpha,
                        Point2.x, Point2.y, Point2.z, Color.r, Color.g, Color.b, Alpha};

    vw_Draw3D(ePrimitiveType::LINES, 2, RI_3f_XYZ | RI_4f_COLOR, DrawArray, 7 * sizeof(DrawArray[0]));
}

/*
 * Draw box lines.
 * Since this is debug only code, no need in future rendering optimization (vbo/ibo/vao, etc...).
 */
static void DrawBoxLines(const bounding_box &Box, const sVECTOR3D &LocalLocation, const sRGBCOLOR &Color)
{
    vw_PushMatrix();
    vw_Translate(LocalLocation);

    DrawLine(Box[0], Box[1], Color);
    DrawLine(Box[1], Box[2], Color);
    DrawLine(Box[2], Box[3], Color);
    DrawLine(Box[3], Box[0], Color);

    DrawLine(Box[4], Box[5], Color);
    DrawLine(Box[5], Box[6], Color);
    DrawLine(Box[6], Box[7], Color);
    DrawLine(Box[7], Box[4], Color);

    DrawLine(Box[0], Box[4], Color);
    DrawLine(Box[1], Box[5], Color);
    DrawLine(Box[2], Box[6], Color);
    DrawLine(Box[3], Box[7], Color);

    vw_PopMatrix();
}

/*
 * Draw bounding boxes.
 */
static void DrawBoundingBoxes(const sVECTOR3D &Location, const bounding_box &AABB,
                              const sOBB &OBB, const std::vector<sHitBB> &HitBB)
{
    if (BBRenderMode == eRenderBoundingBoxes::None) {
        return;
    }

    static const sRGBCOLOR Red{eRGBCOLOR::red};
    DrawBoxLines(AABB, Location, Red);

    if (BBRenderMode == eRenderBoundingBoxes::AABB_Only) {
        return;
    }

    static const sRGBCOLOR Green{eRGBCOLOR::green};
    DrawBoxLines(OBB.Box, Location + OBB.Location, Green);

    if (BBRenderMode == eRenderBoundingBoxes::AABB_And_OBB || HitBB.empty()) {
        return;
    }

    static const sRGBCOLOR Blue{eRGBCOLOR::blue};
    for (const auto &tmpHitBB : HitBB) {
        DrawBoxLines(tmpHitBB.Box, Location + tmpHitBB.Location, Blue);
    }
}

/*
 * Set bounding boxes render mode.
 */
void SetObjectsBBRenderMode(eRenderBoundingBoxes Mode)
{
    BBRenderMode = Mode;
}

/*
 * Fill status draw array for line.
 */
static void FillStatusDrawArray(float SizeXStart, float SizeXEnd, float SizeY,
                                const sRGBCOLOR &Color, float (&DrawArray)[28])
{
    unsigned int DrawArrayIndex{0};
    constexpr float Alpha{1.0f};

    auto FillVertex = [&] (float X, float Y) {
        DrawArray[DrawArrayIndex++] = X;
        DrawArray[DrawArrayIndex++] = Y;
        DrawArray[DrawArrayIndex++] = 0.0f; // Z
        DrawArray[DrawArrayIndex++] = Color.r;
        DrawArray[DrawArrayIndex++] = Color.g;
        DrawArray[DrawArrayIndex++] = Color.b;
        DrawArray[DrawArrayIndex++] = Alpha;
    };

    FillVertex(SizeXStart, -SizeY);
    FillVertex(SizeXStart, SizeY);
    FillVertex(SizeXEnd, -SizeY);
    FillVertex(SizeXEnd, SizeY);
}

/*
 * Draw status near 3d model in space.
 */
static void DrawObjectStatus(const sVECTOR3D &Location, float Width, const sRGBCOLOR &Color,
                             float CurrentValue, float InitialValue)
{
    vw_PushMatrix();
    sVECTOR3D CurrentCameraRotation;
    vw_GetCameraRotation(&CurrentCameraRotation);
    vw_Translate(Location);
    vw_Rotate(-180 + CurrentCameraRotation.y, 0.0f, 1.0f, 0.0f);
    vw_Rotate(-CurrentCameraRotation.x, 1.0f, 0.0f, 0.0f);

    // TRIANGLE_STRIP, RI_3f_XYZ | RI_4f_COLOR = 4 * (3 + 4) = 28
    float DrawArray[28];

    // background
    float SizeX = Width / 2.5f + 0.1f;
    FillStatusDrawArray(-SizeX, SizeX, 0.35f, sRGBCOLOR{eRGBCOLOR::black}, DrawArray);
    vw_Draw3D(ePrimitiveType::TRIANGLE_STRIP, 4, RI_3f_XYZ | RI_4f_COLOR, DrawArray, 7 * sizeof(DrawArray[0]));

    // status
    float SizeXStart = Width / 2.5f - 0.8f * Width * CurrentValue / InitialValue;
    float SizeXEnd = Width / 2.5f;
    FillStatusDrawArray(SizeXStart, SizeXEnd, 0.25f, Color, DrawArray);
    vw_Draw3D(ePrimitiveType::TRIANGLE_STRIP, 4, RI_3f_XYZ | RI_4f_COLOR, DrawArray, 7 * sizeof(DrawArray[0]));

    vw_PopMatrix();
}

/*
 * Draw.
 */
void cObject3D::Draw(bool VertexOnlyPass, bool ShadowMap)
{
    if (Chunks.empty()) {
        return;
    }

    bool NeedOnePieceDraw{false};
    if (PromptDrawDist2 >= 0.0f) {
        sVECTOR3D CurrentCameraLocation;
        vw_GetCameraLocation(&CurrentCameraLocation);
        float PromptDrawRealDist2 = (Location.x - CurrentCameraLocation.x) * (Location.x - CurrentCameraLocation.x) +
                                    (Location.y - CurrentCameraLocation.y) * (Location.y - CurrentCameraLocation.y) +
                                    (Location.z - CurrentCameraLocation.z) * (Location.z - CurrentCameraLocation.z);

        int LightsCount = vw_CalculateAllPointLightsAttenuation(Location, Radius * Radius, nullptr);

        if (PromptDrawRealDist2 > PromptDrawDist2) {
            if (LightsCount <= GameConfig().MaxPointLights) {
                NeedOnePieceDraw = true;
            } else {
                NeedOnePieceDraw = false;
            }

            if (InternalLights >= LightsCount) {
                NeedOnePieceDraw = true;
            }
        } else {
            if (LightsCount <= GameConfig().MaxPointLights) {
                NeedOnePieceDraw = true;
            }
        }
    }

    // fast rendering for shadows map generation, without textures, shaders, etc
    // make sure, we call this one _before_ any camera/frustum checks, since not visible
    // for us 3D model could also drop the shadow on visible for us part of scene
    if (VertexOnlyPass) {
        vw_PushMatrix();

        vw_Translate(Location);
        vw_Rotate(Rotation.z, 0.0f, 0.0f, 1.0f);
        vw_Rotate(Rotation.y, 0.0f, 1.0f, 0.0f);
        vw_Rotate(Rotation.x, 1.0f, 0.0f, 0.0f);

        if (NeedOnePieceDraw) {
            unsigned DrawVertexCount{GlobalIndexArrayCount};
            if (!DrawVertexCount) {
                DrawVertexCount = GlobalVertexArrayCount;
            }

            vw_Draw3D(ePrimitiveType::TRIANGLES, DrawVertexCount, RI_3f_XYZ, GlobalVertexArray.get(),
                      Chunks[0].VertexStride * sizeof(float), GlobalVBO, 0,
                      GlobalIndexArray.get(), GlobalIBO, GlobalVAO);
        } else {
            for (auto &tmpChunk : Chunks) {
                vw_PushMatrix();

                vw_Translate(tmpChunk.Location);
                vw_Rotate(tmpChunk.Rotation.z, 0.0f, 0.0f, 1.0f);
                vw_Rotate(tmpChunk.Rotation.y, 0.0f, 1.0f, 0.0f);
                vw_Rotate(tmpChunk.Rotation.x, 1.0f, 0.0f, 0.0f);

                if (tmpChunk.NeedGeometryAnimation) {
                    vw_Rotate(tmpChunk.GeometryAnimation.z, 0.0f, 0.0f, 1.0f);
                    vw_Rotate(tmpChunk.GeometryAnimation.y, 0.0f, 1.0f, 0.0f);
                    vw_Rotate(tmpChunk.GeometryAnimation.x, 1.0f, 0.0f, 0.0f);
                }

                if (tmpChunk.ShaderType == 2) {
                    if (auto sharedGLSL = GLSLShaderType2.lock()) {
                        vw_UseShaderProgram(sharedGLSL);
                        vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 0), 0);
                        vw_Uniform1f(vw_GetShaderUniformLocation(sharedGLSL, 1), Chunks[0].ShaderData[0]);
                        vw_Uniform1f(vw_GetShaderUniformLocation(sharedGLSL, 2), Chunks[0].ShaderData[1]);
                    }
                }

                vw_Draw3D(ePrimitiveType::TRIANGLES, tmpChunk.VertexQuantity, RI_3f_XYZ, tmpChunk.VertexArray.get(),
                          tmpChunk.VertexStride * sizeof(float), tmpChunk.VBO,
                          tmpChunk.RangeStart, tmpChunk.IndexArray.get(), tmpChunk.IBO, tmpChunk.VAO);

                if (tmpChunk.ShaderType == 2 && !GLSLShaderType2.expired()) {
                    vw_StopShaderProgram();
                }

                vw_PopMatrix();
            }
        }

        vw_PopMatrix();
        return;
    }

    if (!vw_BoxInFrustum(Location + AABB[6], Location + AABB[0])) {
        if (DeleteAfterLeaveScene == eDeleteAfterLeaveScene::showed) {
            DeleteAfterLeaveScene = eDeleteAfterLeaveScene::need_delete;
        }
        return;
    }

    if (DeleteAfterLeaveScene == eDeleteAfterLeaveScene::enabled) {
        DeleteAfterLeaveScene = eDeleteAfterLeaveScene::showed;
    }
    if (DeleteAfterLeaveScene == eDeleteAfterLeaveScene::wait_delay) {
        DeleteAfterLeaveScene = eDeleteAfterLeaveScene::showed;
        Lifetime = -1.0f;
    }

    GLtexture CurrentNormalMap{0};
    std::weak_ptr<cGLSL> CurrentGLSL{};
    int NeedNormalMapping{0};
    float Matrix[16];
    vw_GetMatrix(eMatrixPname::MODELVIEW, Matrix);

    vw_PushMatrix();

    vw_Translate(Location);
    vw_Rotate(Rotation.z, 0.0f, 0.0f, 1.0f);
    vw_Rotate(Rotation.y, 0.0f, 1.0f, 0.0f);
    vw_Rotate(Rotation.x, 1.0f, 0.0f, 0.0f);

    bool N1 = false;
    for (auto &tmpChunk : Chunks) {
        if (tmpChunk.DrawType == eModel3DDrawType::Blend) {
            N1 = true;
        }
    }

    vw_MaterialV(eMaterialParameter::DIFFUSE, Diffuse);
    vw_MaterialV(eMaterialParameter::AMBIENT, Ambient);
    vw_MaterialV(eMaterialParameter::SPECULAR, Specular);
    vw_MaterialV(eMaterialParameter::SHININESS, Power);

    if (!NeedCullFaces) {
        vw_CullFace(eCullFace::NONE);
    }
    if (NeedAlphaTest) {
        vw_SetTextureAlphaTest(true, eCompareFunc::GREATER, 0.4f);
    }

    if (NeedOnePieceDraw) {
        vw_BindTexture(0, Texture[0]);

        if (!TextureIllum.empty() && TextureIllum[0]) {
            vw_BindTexture(1, TextureIllum[0]);
            vw_SetTextureEnvMode(eTextureEnvMode::COMBINE);
            vw_SetTextureBlendMode(eTextureCombinerName::COMBINE_RGB, eTextureCombinerOp::ADD);
        }

        NeedNormalMapping = 0;
        if (GameConfig().UseGLSL120 && !NormalMap.empty() && NormalMap[0]) {
            NeedNormalMapping = 1;
            CurrentNormalMap = NormalMap[0];
            vw_BindTexture(3, CurrentNormalMap);
        }

        vw_CheckAndActivateAllLights(Location, Radius*Radius, 1, GameConfig().MaxPointLights, Matrix);

        if (GameConfig().UseGLSL120) {
            std::weak_ptr<cGLSL> CurrentObject3DGLSL{};

            // FIXME we know what exactly we have, why we need shaders setup in this way?
            if (Chunks[0].ShaderType == 1 && ShadowMap) {
                Chunks[0].ShaderType = 3;
            }
            if (Chunks[0].ShaderType == 3 && !ShadowMap) {
                Chunks[0].ShaderType = 1;
            }

            switch (Chunks[0].ShaderType) {
            case 1:
                CurrentObject3DGLSL = GLSLShaderType1;
                break;
            case 2:
                CurrentObject3DGLSL = GLSLShaderType2;
                break;
            case 3:
                CurrentObject3DGLSL = GLSLShaderType3;
                break;
            }

            if (CurrentGLSL.lock() != CurrentObject3DGLSL.lock()) {
                if (!CurrentGLSL.expired()) {
                    vw_StopShaderProgram();
                }

                CurrentGLSL = CurrentObject3DGLSL;

                if (!CurrentGLSL.expired()) {
                    vw_UseShaderProgram(CurrentGLSL);
                }
            }

            if (auto sharedGLSL = CurrentObject3DGLSL.lock()) {
                switch (Chunks[0].ShaderType) {
                case 1: // per pixel light
                    vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 0), 0);
                    vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 1), 1);
                    if (!TextureIllum.empty() && TextureIllum[0]) {
                        vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 2), 1);
                    } else {
                        vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 2), 0);
                    }

                    vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 3), 3);
                    vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 4), NeedNormalMapping);
                    break;

                case 2: // explosion
                    vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 0), 0);
                    vw_Uniform1f(vw_GetShaderUniformLocation(sharedGLSL, 1), Chunks[0].ShaderData[0]);
                    vw_Uniform1f(vw_GetShaderUniformLocation(sharedGLSL, 2), Chunks[0].ShaderData[1]);
                    break;

                case 3: // shadow map
                    vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 0), 0);
                    vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 1), 1);
                    if (!TextureIllum.empty() && TextureIllum[0]) {
                        vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 2), 1);
                    } else {
                        vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 2), 0);
                    }

                    vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 3), 2);
                    vw_Uniform1f(vw_GetShaderUniformLocation(sharedGLSL, 4), ShadowMap_Get_xPixelOffset());
                    vw_Uniform1f(vw_GetShaderUniformLocation(sharedGLSL, 5), ShadowMap_Get_yPixelOffset());
                    vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 6), 3);
                    vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 7), NeedNormalMapping);
                    break;
                }
            }
        }

        unsigned DrawVertexCount{GlobalIndexArrayCount};
        if (!DrawVertexCount) {
            DrawVertexCount = GlobalVertexArrayCount;
        }

        vw_Draw3D(ePrimitiveType::TRIANGLES, DrawVertexCount, Chunks[0].VertexFormat, GlobalVertexArray.get(),
                  Chunks[0].VertexStride * sizeof(float), GlobalVBO, 0,
                  GlobalIndexArray.get(), GlobalIBO, GlobalVAO);

        vw_DeActivateAllLights();
    } else {
        GLtexture CurrentTexture{0};

        for (unsigned int i = 0; i < Chunks.size(); i++) {
            if (!HitBB.empty()) {
                sVECTOR3D Min, Max;
                Min.x = Max.x = HitBB[i].Box[0].x + HitBB[i].Location.x;
                Min.y = Max.y = HitBB[i].Box[0].y + HitBB[i].Location.y;
                Min.z = Max.z = HitBB[i].Box[0].z + HitBB[i].Location.z;
                for (int j = 0; j < 8; j++) {
                    if (Min.x > HitBB[i].Box[j].x + HitBB[i].Location.x) {
                        Min.x = HitBB[i].Box[j].x + HitBB[i].Location.x;
                    }
                    if (Min.y > HitBB[i].Box[j].y + HitBB[i].Location.y) {
                        Min.y = HitBB[i].Box[j].y + HitBB[i].Location.y;
                    }
                    if (Min.z > HitBB[i].Box[j].z + HitBB[i].Location.z) {
                        Min.z = HitBB[i].Box[j].z + HitBB[i].Location.z;
                    }
                    if (Max.x < HitBB[i].Box[j].x + HitBB[i].Location.x) {
                        Max.x = HitBB[i].Box[j].x + HitBB[i].Location.x;
                    }
                    if (Max.y < HitBB[i].Box[j].y + HitBB[i].Location.y) {
                        Max.y = HitBB[i].Box[j].y + HitBB[i].Location.y;
                    }
                    if (Max.z < HitBB[i].Box[j].z + HitBB[i].Location.z) {
                        Max.z = HitBB[i].Box[j].z + HitBB[i].Location.z;
                    }
                }

                if (!vw_BoxInFrustum(Location + Min, Location + Max)) {
                    continue;
                }
            }

            if (CurrentTexture != Texture[i]) {
                vw_BindTexture(0, Texture[i]);

                if (Chunks[i].NeedTextureAnimation) {
                    vw_MatrixMode(eMatrixMode::TEXTURE);
                    vw_LoadIdentity();
                    vw_Translate(Chunks[i].TextureAnimation);
                    vw_MatrixMode(eMatrixMode::MODELVIEW);
                }

                if (TextureIllum.size() > static_cast<unsigned>(i) && TextureIllum[i]) {
                    vw_BindTexture(1, TextureIllum[i]);
                    vw_SetTextureEnvMode(eTextureEnvMode::COMBINE);
                    vw_SetTextureBlendMode(eTextureCombinerName::COMBINE_RGB, eTextureCombinerOp::ADD);
                }

                NeedNormalMapping = 0;
                if (GameConfig().UseGLSL120 && NormalMap.size() > static_cast<unsigned>(i)) {
                    if (NormalMap[i]) {
                        NeedNormalMapping = 1;
                        CurrentNormalMap = NormalMap[i];
                        vw_BindTexture(3, CurrentNormalMap);
                    } else if (CurrentNormalMap) {
                        vw_BindTexture(3, 0);
                        CurrentNormalMap = 0;
                    }
                }

                CurrentTexture = Texture[i];
            }

            vw_PushMatrix();

            vw_Translate(Chunks[i].Location);
            vw_Rotate(Chunks[i].Rotation.z, 0.0f, 0.0f, 1.0f);
            vw_Rotate(Chunks[i].Rotation.y, 0.0f, 1.0f, 0.0f);
            vw_Rotate(Chunks[i].Rotation.x, 1.0f, 0.0f, 0.0f);

            if (Chunks[i].NeedGeometryAnimation) {
                vw_Rotate(Chunks[i].GeometryAnimation.z, 0.0f, 0.0f, 1.0f);
                vw_Rotate(Chunks[i].GeometryAnimation.y, 0.0f, 1.0f, 0.0f);
                vw_Rotate(Chunks[i].GeometryAnimation.x, 1.0f, 0.0f, 0.0f);
            }

            if (!HitBB.empty()) {
                vw_CheckAndActivateAllLights(Location + HitBB[i].Location, HitBB[i].Radius2, 1, GameConfig().MaxPointLights, Matrix);
            } else {
                vw_CheckAndActivateAllLights(Location, Radius * Radius, 1, GameConfig().MaxPointLights, Matrix);
            }

            // for planet's clouds
            if (N1) {
                vw_PolygonOffset(true, 2.0f, 2.0f);
            }

            if (Chunks[i].DrawType == eModel3DDrawType::Blend) {
                vw_SetTextureAlphaTest(true, eCompareFunc::GREATER, 0.01f);
                vw_SetTextureBlend(true, eTextureBlendFactor::SRC_ALPHA, eTextureBlendFactor::ONE);
                vw_PolygonOffset(true, 1.0f, 1.0f);
            }

            if (GameConfig().UseGLSL120) {
                std::weak_ptr<cGLSL> CurrentObject3DGLSL{};

                // FIXME we know what exactly we have, why we need shaders setup in this way?
                if (Chunks[i].ShaderType == 1 && ShadowMap) {
                    Chunks[i].ShaderType = 3;
                }
                if (Chunks[i].ShaderType == 3 && !ShadowMap) {
                    Chunks[i].ShaderType = 1;
                }

                switch (Chunks[i].ShaderType) {
                case 1:
                    CurrentObject3DGLSL = GLSLShaderType1;
                    break;
                case 2:
                    CurrentObject3DGLSL = GLSLShaderType2;
                    break;
                case 3:
                    CurrentObject3DGLSL = GLSLShaderType3;
                    break;
                }

                if (CurrentGLSL.lock() != CurrentObject3DGLSL.lock()) {
                    if (!CurrentGLSL.expired()) {
                        vw_StopShaderProgram();
                    }

                    CurrentGLSL = CurrentObject3DGLSL;

                    if (!CurrentGLSL.expired()) {
                        vw_UseShaderProgram(CurrentGLSL);
                    }
                }

                if (auto sharedGLSL = CurrentObject3DGLSL.lock()) {
                    switch (Chunks[i].ShaderType) {
                    case 1: // per pixel light
                        vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 0), 0);
                        vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 1), 1);
                        if (!TextureIllum.empty() && TextureIllum[0]) {
                            vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 2), 1);
                        } else {
                            vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 2), 0);
                        }

                        vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 3), 3);
                        vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 4), NeedNormalMapping);
                        break;

                    case 2: // explosion
                        vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 0), 0);
                        vw_Uniform1f(vw_GetShaderUniformLocation(sharedGLSL, 1), Chunks[0].ShaderData[0]);
                        vw_Uniform1f(vw_GetShaderUniformLocation(sharedGLSL, 2), Chunks[0].ShaderData[1]);
                        break;

                    case 3: // shadow map
                        vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 0), 0);
                        vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 1), 1);
                        if (!TextureIllum.empty() && TextureIllum[0]) {
                            vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 2), 1);
                        } else {
                            vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 2), 0);
                        }

                        vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 3), 2);
                        vw_Uniform1f(vw_GetShaderUniformLocation(sharedGLSL, 4), ShadowMap_Get_xPixelOffset());
                        vw_Uniform1f(vw_GetShaderUniformLocation(sharedGLSL, 5), ShadowMap_Get_yPixelOffset());
                        vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 6), 3);
                        vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 7), NeedNormalMapping);
                        break;
                    }
                }
            }

            vw_Draw3D(ePrimitiveType::TRIANGLES, Chunks[i].VertexQuantity, Chunks[i].VertexFormat, Chunks[i].VertexArray.get(),
                      Chunks[i].VertexStride * sizeof(float), Chunks[i].VBO,
                      Chunks[i].RangeStart, Chunks[i].IndexArray.get(), Chunks[i].IBO, Chunks[i].VAO);

            if (Chunks[i].DrawType == eModel3DDrawType::Blend) {
                vw_SetTextureAlphaTest(false, eCompareFunc::ALWAYS, 0);
                vw_SetTextureBlend(false, eTextureBlendFactor::ONE, eTextureBlendFactor::ZERO);
                vw_PolygonOffset(false, 0.0f, 0.0f);
            }

            vw_DeActivateAllLights();

            if (Chunks[i].NeedTextureAnimation) {
                vw_BindTexture(0, 0);
                vw_MatrixMode(eMatrixMode::TEXTURE);
                vw_LoadIdentity();
                vw_MatrixMode(eMatrixMode::MODELVIEW);
                CurrentTexture = 0;
            }

            vw_PopMatrix();
        }
    }

    if (GameConfig().UseGLSL120) {
        vw_StopShaderProgram();
    }

    if (CurrentNormalMap) {
        vw_BindTexture(3, 0);
    }
    vw_BindTexture(1, 0);
    vw_BindTexture(0, 0);
    if (NeedAlphaTest) {
        vw_SetTextureAlphaTest(false, eCompareFunc::ALWAYS, 0);
    }
    if (!NeedCullFaces) {
        vw_CullFace(eCullFace::BACK);
    }
    vw_PopMatrix();

#ifndef NDEBUG
    // debug info, line number in script file
    if (!ScriptLineNumberUTF32.empty()) {
        vw_DrawText3DUTF32(Location.x, Location.y + AABB[0].y, Location.z, ScriptLineNumberUTF32);
    }
#endif // NDEBUG

    DrawBoundingBoxes(Location, AABB, OBB, HitBB);

    // TODO why we need ShowStatus if we could use ArmorInitialStatus < 0.0f for this?
    if (!ShowStatus
        || ArmorInitialStatus <= 0.0f
        || (ArmorCurrentStatus == ArmorInitialStatus
            && ShieldCurrentStatus == ShieldInitialStatus
            && !ShowStatusAllTime)) {
        return;
    }

    // even if shield recharged - don't hide object's status any more
    ShowStatusAllTime = true;

    DrawObjectStatus(sVECTOR3D{Location.x, Location.y + AABB[0].y + 0.7f, Location.z},
                     Width, sRGBCOLOR{eRGBCOLOR::red}, ArmorCurrentStatus, ArmorInitialStatus);
    if (ShieldInitialStatus > 0.0f) {
        DrawObjectStatus(sVECTOR3D{Location.x, Location.y + AABB[0].y + 1.75f, Location.z},
                         Width, sRGBCOLOR{0.1f, 0.7f, 1.0f}, ShieldCurrentStatus, ShieldInitialStatus);
    }
    vw_BindTexture(0, 0);
}

/*
 * Update.
 */
bool cObject3D::Update(float Time)
{
    if (TimeLastUpdate == -1.0f) {
        TimeLastUpdate = Time;
        return true;
    }

    if (DeleteAfterLeaveScene == eDeleteAfterLeaveScene::need_delete) {
        Lifetime = DeleteAfterLeaveSceneDelay;
        DeleteAfterLeaveScene = eDeleteAfterLeaveScene::wait_delay;
    }

    TimeDelta = Time - TimeLastUpdate;
    if (TimeDelta == 0.0f) {
        return true;
    }

    TimeLastUpdate = Time;

    if (Lifetime > -1.0f) {
        Lifetime -= TimeDelta;
        if (Lifetime <= 0.0f) {
            return false;
        }
    }

    if (ShieldInitialStatus > 0.0f && ShieldCurrentStatus < ShieldInitialStatus) {
        ShieldCurrentStatus += ShieldRechargeRate * TimeDelta;
        if (ShieldCurrentStatus > ShieldInitialStatus) {
            ShieldCurrentStatus = ShieldInitialStatus;
        }
    }

    // if we have TimeSheet with actions and this is not a cycled entry
    if (!TimeSheetList.empty() && TimeSheetList.front().Time > -1.0f) {
        TimeSheetList.front().Time -= TimeDelta;
        // if this entry is out of time, remove it
        if (TimeSheetList.front().Time <= 0.0f) {
            // correct time delta
            if (TimeSheetList.front().Time < 0.0f) {
                TimeDelta += TimeSheetList.front().Time;
            }
            TimeSheetList.pop_front();
        }
    }
    // should be unpacked
    if (!TimeSheetList.empty() && TimeSheetList.front().AI_Mode != 0) {
        InterAIMode(TimeSheetList);
        // since we already unpack this entry, remove it
        TimeSheetList.pop_front();
    }

    return true;
}

} // astromenace namespace
} // viewizard namespace
