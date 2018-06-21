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


	Web Site: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "../core/core.h"

namespace {

// empirical found "load value" for each sfx asset
// small value mean that asset loads fast, big value - slow
constexpr unsigned SFXLoadValue{20};

// make sure, that all sfx are added in order to load them before first access,
// otherwise they will be load on first access (that may lag the game process)
const std::vector<std::string> SFXAssetArray{
	{"sfx/weapon1probl.wav"},
	{"sfx/weapon2probl.wav"},
	{"sfx/weapon3probl.wav"},
	{"sfx/weapon4probl.wav"},
	{"sfx/weapon5probl.wav"},
	{"sfx/explosion1.wav"},
	{"sfx/explosion2.wav"},
	{"sfx/explosion3.wav"},
	{"sfx/explosion4.wav"},
	{"sfx/weaponfire1.wav"},
	{"sfx/weaponfire2.wav"},
	{"sfx/weaponfire3.wav"},
	{"sfx/weaponfire4.wav"},
	{"sfx/weaponfire5.wav"},
	{"sfx/weaponfire6.wav"},
	{"sfx/weaponfire7.wav"},
	{"sfx/weaponfire8.wav"},
	{"sfx/weaponfire9.wav"},
	{"sfx/weaponfire10.wav"},
	{"sfx/weaponfire11.wav"},
	{"sfx/weaponfire12.wav"},
	{"sfx/weaponfire13.wav"},
	{"sfx/weaponfire14.wav"},
	{"sfx/weaponfire15.wav"},
	{"sfx/weaponfire16.wav"},
	{"sfx/weaponfire17.wav"},
	{"sfx/weaponfire18.wav"},
	{"sfx/weaponfire19.wav"},
	{"sfx/kinetichit.wav"},
	{"sfx/ionhit.wav"},
	{"sfx/plasmahit.wav"},
	{"sfx/antimaterhit.wav"},
	{"sfx/gausshit.wav"},
	{"sfx/lowlife.wav"}
};

} // unnamed namespace


/*
 * Get all audio assets value.
 */
unsigned GetAudioAssetsValue()
{
	return SFXAssetArray.size() * SFXLoadValue;
}

/*
 * Cycle with function call on each audio asset load.
 */
void ForEachAudioAssetLoad(std::function<void (unsigned AssetValue)> function)
{
	if (!vw_GetAudioStatus())
		return;

	for (auto &tmpAsset : SFXAssetArray) {
		vw_LoadSoundBuffer(tmpAsset);
		function(SFXLoadValue);
	}
}
