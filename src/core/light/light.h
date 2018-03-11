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

#ifndef LIGHT_H
#define LIGHT_H

#include "../base.h"

struct sVECTOR3D;

// We can't use "enum class" as std::unordered_multimap key,
// OS X AppleClang 8.1.0.8020042 don't like this:
// error: implicit instantiation of undefined template 'std::__1::hash<eLightType>'
// This is sad, since Linux GCC and Clang works well.
constexpr int LightType_Directional{0};	// located far (sun, stars, etc)
constexpr int LightType_Point{1};	// located close (engines, weapon flashes, etc)


struct sLight {
	// Activate and setup for proper light type (OpenGL-related).
	bool Activate(int CurrentLightNum, const std::array<float, 16> &Matrix);
	// Deactivate (OpenGL-related).
	void DeActivate();
	// Set location.
	void SetLocation(sVECTOR3D NewLocation);

	// Light's color.
	float Diffuse[4]{0.0f, 0.0f, 0.0f, 1.0f};
	float Specular[4]{0.0f, 0.0f, 0.0f, 1.0f};
	float Ambient[4]{0.0f, 0.0f, 0.0f, 1.0f};
	// Maximum color (for color deviation calculations).
	float DiffuseMax[4];
	float SpecularMax[4];
	// Attenuations.
	float ConstantAttenuation{0.0f};
	float LinearAttenuation{0.0f};
	float QuadraticAttenuation{0.0f};
	// Attenuations base (for particle system effects).
	float LinearAttenuationBase{0.0f};
	float QuadraticAttenuationBase{0.0f};
	// Direction.
	sVECTOR3D Direction{0.0f, 0.0f, 0.0f};
	// Location (for point lights only).
	sVECTOR3D Location{0.0f, 0.0f, 0.0f};
	// On/Off switch, in this case Off mean On=false.
	bool On{true};
	// OpenGL-related.
	int RealLightNum{-1};
};


// Activate proper lights for particular object (presented by location and radius^2).
int vw_CheckAndActivateAllLights(int &Type1, int &Type2, const sVECTOR3D &Location, float Radius2,
				 int DirLimit, int PointLimit, const std::array<float, 16> &Matrix);
// Calculate affected lights counter and create sorted map with affected lights.
int vw_CalculateAllPointLightsAttenuation(const sVECTOR3D &Location, float Radius2,
					  std::multimap<float, sLight*> *AffectedLightsMap);
// Deactivate all lights.
void vw_DeActivateAllLights();
// Release light.
void vw_ReleaseLight(sLight *Light);
// Release all lights.
void vw_ReleaseAllLights();
// Create light.
sLight *vw_CreateLight(int Type);
// Create point light with initialization.
sLight *vw_CreatePointLight(const sVECTOR3D &Location, float R, float G, float B, float Linear, float Quadratic);
// Get main direct light. Usually, first one is the main.
sLight *vw_GetMainDirectLight();

#endif // LIGHT_H
