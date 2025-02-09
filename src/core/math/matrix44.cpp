/****************************************************************************

    AstroMenace
    Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
    Copyright (C) 2006-2025 Mikhail Kurinnoi, Viewizard


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

#include "math.h"
#include <cmath>

namespace viewizard {

/*
 * Setup matrix identity.
 */
void vw_Matrix44Identity(float (&Matrix44)[16])
{
    Matrix44[0 ] = 1.0f;
    Matrix44[1 ] = 0.0f;
    Matrix44[2 ] = 0.0f;
    Matrix44[3 ] = 0.0f;

    Matrix44[4 ] = 0.0f;
    Matrix44[5 ] = 1.0f;
    Matrix44[6 ] = 0.0f;
    Matrix44[7 ] = 0.0f;

    Matrix44[8 ] = 0.0f;
    Matrix44[9 ] = 0.0f;
    Matrix44[10] = 1.0f;
    Matrix44[11] = 0.0f;

    Matrix44[12] = 0.0f;
    Matrix44[13] = 0.0f;
    Matrix44[14] = 0.0f;
    Matrix44[15] = 1.0f;
}

/*
 * Matrix multiplication.
 */
void vw_Matrix44Mult(float (&DstMatrix44)[16], const float (&SrcMatrix44)[16])
{
    float tmp[16]{DstMatrix44[0 ], DstMatrix44[1 ], DstMatrix44[2 ], DstMatrix44[3 ],
                  DstMatrix44[4 ], DstMatrix44[5 ], DstMatrix44[6 ], DstMatrix44[7 ],
                  DstMatrix44[8 ], DstMatrix44[9 ], DstMatrix44[10], DstMatrix44[11],
                  DstMatrix44[12], DstMatrix44[13], DstMatrix44[14], DstMatrix44[15]};

    DstMatrix44[0 ] = SrcMatrix44[0 ] * tmp[0] + SrcMatrix44[1 ] * tmp[4] +
                      SrcMatrix44[2 ] * tmp[8 ] + SrcMatrix44[3 ] * tmp[12];
    DstMatrix44[1 ] = SrcMatrix44[0 ] * tmp[1] + SrcMatrix44[1 ] * tmp[5] +
                      SrcMatrix44[2 ] * tmp[9 ] + SrcMatrix44[3 ] * tmp[13];
    DstMatrix44[2 ] = SrcMatrix44[0 ] * tmp[2] + SrcMatrix44[1 ] * tmp[6] +
                      SrcMatrix44[2 ] * tmp[10] + SrcMatrix44[3 ] * tmp[14];
    DstMatrix44[3 ] = SrcMatrix44[0 ] * tmp[3] + SrcMatrix44[1 ] * tmp[7] +
                      SrcMatrix44[2 ] * tmp[11] + SrcMatrix44[3 ] * tmp[15];

    DstMatrix44[4 ] = SrcMatrix44[4 ] * tmp[0] + SrcMatrix44[5 ] * tmp[4] +
                      SrcMatrix44[6 ] * tmp[8 ] + SrcMatrix44[7 ] * tmp[12];
    DstMatrix44[5 ] = SrcMatrix44[4 ] * tmp[1] + SrcMatrix44[5 ] * tmp[5] +
                      SrcMatrix44[6 ] * tmp[9 ] + SrcMatrix44[7 ] * tmp[13];
    DstMatrix44[6 ] = SrcMatrix44[4 ] * tmp[2] + SrcMatrix44[5 ] * tmp[6] +
                      SrcMatrix44[6 ] * tmp[10] + SrcMatrix44[7 ] * tmp[14];
    DstMatrix44[7 ] = SrcMatrix44[4 ] * tmp[3] + SrcMatrix44[5 ] * tmp[7] +
                      SrcMatrix44[6 ] * tmp[11] + SrcMatrix44[7 ] * tmp[15];

    DstMatrix44[8 ] = SrcMatrix44[8 ] * tmp[0] + SrcMatrix44[9 ] * tmp[4] +
                      SrcMatrix44[10] * tmp[8 ] + SrcMatrix44[11] * tmp[12];
    DstMatrix44[9 ] = SrcMatrix44[8 ] * tmp[1] + SrcMatrix44[9 ] * tmp[5] +
                      SrcMatrix44[10] * tmp[9 ] + SrcMatrix44[11] * tmp[13];
    DstMatrix44[10] = SrcMatrix44[8 ] * tmp[2] + SrcMatrix44[9 ] * tmp[6] +
                      SrcMatrix44[10] * tmp[10] + SrcMatrix44[11] * tmp[14];
    DstMatrix44[11] = SrcMatrix44[8 ] * tmp[3] + SrcMatrix44[9 ] * tmp[7] +
                      SrcMatrix44[10] * tmp[11] + SrcMatrix44[11] * tmp[15];

    DstMatrix44[12] = SrcMatrix44[12] * tmp[0] + SrcMatrix44[13] * tmp[4] +
                      SrcMatrix44[14] * tmp[8 ] + SrcMatrix44[15] * tmp[12];
    DstMatrix44[13] = SrcMatrix44[12] * tmp[1] + SrcMatrix44[13] * tmp[5] +
                      SrcMatrix44[14] * tmp[9 ] + SrcMatrix44[15] * tmp[13];
    DstMatrix44[14] = SrcMatrix44[12] * tmp[2] + SrcMatrix44[13] * tmp[6] +
                      SrcMatrix44[14] * tmp[10] + SrcMatrix44[15] * tmp[14];
    DstMatrix44[15] = SrcMatrix44[12] * tmp[3] + SrcMatrix44[13] * tmp[7] +
                      SrcMatrix44[14] * tmp[11] + SrcMatrix44[15] * tmp[15];
}

/*
 * Calculate translation matrix by new location point.
 */
void vw_Matrix44Translate(float (&Matrix44)[16], const sVECTOR3D &Location)
{
    Matrix44[12] += Location.x;
    Matrix44[13] += Location.y;
    Matrix44[14] += Location.z;
}

/*
 * Create rotation matrix.
 */
void vw_Matrix44CreateRotate(float (&Matrix44)[16], const sVECTOR3D &Angle)
{
    constexpr float DegToRadFactor = 0.0174532925f; // conversion factor to convert degrees to radians

    if (Angle.z != 0.0f && Angle.x == 0.0f && Angle.y == 0.0f) {
        float a = -Angle.z * DegToRadFactor;
        float c = cosf(a);
        float s = sinf(a);
        Matrix44[0] = c;
        Matrix44[1] = -s;
        Matrix44[4] = s;
        Matrix44[5] = c;
        Matrix44[2] = Matrix44[3] = Matrix44[6] = Matrix44[7] = Matrix44[8] = Matrix44[9] = 0.0f;
        Matrix44[11] = Matrix44[12] = Matrix44[13] = Matrix44[14] = 0.0f;
        Matrix44[15] = Matrix44[10] = 1.0f;
    } else if (Angle.y != 0.0f && Angle.x == 0.0f && Angle.z == 0.0f) {
        float a = -Angle.y * DegToRadFactor;
        float c = cosf(a);
        float s = sinf(a);
        Matrix44[0] = c;
        Matrix44[2] = s;
        Matrix44[8] = -s;
        Matrix44[10] = c;
        Matrix44[1] = Matrix44[3] = Matrix44[4] = Matrix44[6] = Matrix44[7] = 0.0f;
        Matrix44[9] = Matrix44[11] = Matrix44[12] = Matrix44[13] = Matrix44[14] = 0.0f;
        Matrix44[15] = Matrix44[5] = 1.0f;
    } else if (Angle.x != 0.0f && Angle.y == 0.0f && Angle.z == 0.0f) {
        float a = -Angle.x * DegToRadFactor;
        float c = cosf(a);
        float s = sinf(a);
        Matrix44[5] = c;
        Matrix44[6] = -s;
        Matrix44[9] = s;
        Matrix44[10] = c;
        Matrix44[1] = Matrix44[2] = Matrix44[3] = Matrix44[4] = Matrix44[7] = 0.0f;
        Matrix44[8] = Matrix44[11] = Matrix44[12] = Matrix44[13] = Matrix44[14] = 0.0f;
        Matrix44[0] = Matrix44[15] = 1.0f;
    } else {
        // if we need 2 or more angles
        float a = -Angle.x * DegToRadFactor;
        float A = cosf(a);
        float B = sinf(a);
        a = -Angle.y * DegToRadFactor;
        float C = cosf(a);
        float D = sinf(a);
        a = -Angle.z * DegToRadFactor;
        float E = cosf(a);
        float F = sinf(a);
        float AD = A * D;
        float BD = B * D;
        Matrix44[0]  =   C * E;
        Matrix44[1]  =  -C * F;
        Matrix44[2]  =   D;
        Matrix44[4]  =  BD * E + A * F;
        Matrix44[5]  = -BD * F + A * E;
        Matrix44[6]  =  -B * C;
        Matrix44[8]  = -AD * E + B * F;
        Matrix44[9]  =  AD * F + B * E;
        Matrix44[10] =   A * C;
        Matrix44[3]  =  Matrix44[7] = Matrix44[11] = Matrix44[12] = Matrix44[13] = Matrix44[14] = 0.0f;
        Matrix44[15] =  1.0f;
    }
}

/*
 * Create inverted rotation matrix.
 */
void vw_Matrix44InverseRotate(float (&Matrix44)[16])
{
    float tmp[16]{Matrix44[0 ], Matrix44[1 ], Matrix44[2 ], Matrix44[3 ],
                  Matrix44[4 ], Matrix44[5 ], Matrix44[6 ], Matrix44[7 ],
                  Matrix44[8 ], Matrix44[9 ], Matrix44[10], Matrix44[11],
                  Matrix44[12], Matrix44[13], Matrix44[14], Matrix44[15]};

    Matrix44[0 ] =  tmp[0 ];
    Matrix44[1 ] =  tmp[4 ];
    Matrix44[2 ] =  tmp[8 ];

    Matrix44[4 ] =  tmp[1 ];
    Matrix44[5 ] =  tmp[5 ];
    Matrix44[6 ] =  tmp[9 ];

    Matrix44[8 ] =  tmp[2 ];
    Matrix44[9 ] =  tmp[6 ];
    Matrix44[10] =  tmp[10];

    Matrix44[12] = -tmp[12] * tmp[0] - tmp[13] * tmp[1] - tmp[14] * tmp[2 ];
    Matrix44[13] = -tmp[12] * tmp[4] - tmp[13] * tmp[5] - tmp[14] * tmp[6 ];
    Matrix44[14] = -tmp[12] * tmp[8] - tmp[13] * tmp[9] - tmp[14] * tmp[10];
    Matrix44[15] =  tmp[15];
}

/*
 * Calculate point position by transformation matrix.
 */
void vw_Matrix44CalcPoint(sVECTOR3D &Point, const float (&Matrix44)[16])
{
    Point(Matrix44[0] * Point.x + Matrix44[4] * Point.y + Matrix44[8 ] * Point.z + Matrix44[12],
          Matrix44[1] * Point.x + Matrix44[5] * Point.y + Matrix44[9 ] * Point.z + Matrix44[13],
          Matrix44[2] * Point.x + Matrix44[6] * Point.y + Matrix44[10] * Point.z + Matrix44[14]);
}

} // viewizard namespace
