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
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#ifndef ASSETS_TEXTURE_H
#define ASSETS_TEXTURE_H

#include "../core/base.h"
#include "../core/graphics/graphics.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// Get all texture assets load value.
unsigned GetTextureAssetsLoadValue();
// Cycle with function callback on each texture asset load.
void ForEachTextureAssetLoad(std::function<void (unsigned AssetValue)> function);
// Change anisotropy level for all textures that was loaded with anisotropy.
void ChangeTexturesAnisotropyLevel();
// Get preloaded texture asset (preloaded by ForEachTextureAssetLoad() call).
// Note, we don't validate textures, caller should care about call
// ForEachTextureAssetLoad() each time, when this need.
GLtexture GetPreloadedTextureAsset(const std::string &FileName);

} // astromenace namespace
} // viewizard namespace

#endif // ASSETS_TEXTURE_H
