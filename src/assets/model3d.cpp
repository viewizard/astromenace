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
#include "../config/config.h"

namespace {

// empirical found "load value" for each model3d asset
// small value mean that asset loads fast, big value - slow
constexpr unsigned Model3DLoadValue{200};

struct sModel3DAsset {
	std::string FileName{};
	float TriangleSizeLimit{-1.0f};
	bool NeedTangentAndBinormal{false};

	sModel3DAsset(const std::string &_FileName,
		      float _TriangleSizeLimit,
		      bool _NeedTangentAndBinormal) :
		FileName{_FileName},
		TriangleSizeLimit{_TriangleSizeLimit},
		NeedTangentAndBinormal{_NeedTangentAndBinormal}
	{}
};

// make sure, that all model3d are added in order to load them before first access,
// otherwise they will be load on first access (that may lag the game process)
const std::vector<sModel3DAsset> Model3DAssetArray{
	{"models/mine/mine-01.vw3d", 2.0f, false},
	{"models/mine/mine-02.vw3d", 2.0f, false},
	{"models/mine/mine-03.vw3d", 2.0f, false},
	{"models/mine/mine-04.vw3d", 2.0f, false},
	{"models/earthfighter/sf-01.vw3d", 2.0f, false},
	{"models/earthfighter/sf-02.vw3d", 2.0f, false},
	{"models/earthfighter/sf-03.vw3d", 2.0f, false},
	{"models/earthfighter/sf-04.vw3d", 2.0f, false},
	{"models/earthfighter/sf-05.vw3d", 2.0f, false},
	{"models/earthfighter/sf-06.vw3d", 2.0f, false},
	{"models/earthfighter/sf-07.vw3d", 2.0f, false},
	{"models/earthfighter/sf-08.vw3d", 2.0f, false},
	{"models/earthfighter/sf-09.vw3d", 2.0f, false},
	{"models/earthfighter/sf-10.vw3d", 2.0f, false},
	{"models/earthfighter/sf-11.vw3d", 2.0f, false},
	{"models/earthfighter/sf-12.vw3d", 2.0f, false},
	{"models/earthfighter/sf-13.vw3d", 2.0f, false},
	{"models/earthfighter/sf-14.vw3d", 2.0f, false},
	{"models/earthfighter/sf-15.vw3d", 2.0f, false},
	{"models/earthfighter/sf-16.vw3d", 2.0f, false},
	{"models/earthfighter/sf-17.vw3d", 2.0f, false},
	{"models/earthfighter/sf-18.vw3d", 2.0f, false},
	{"models/earthfighter/sf-19.vw3d", 2.0f, false},
	{"models/earthfighter/sf-20.vw3d", 2.0f, false},
	{"models/earthfighter/sf-21.vw3d", 2.0f, false},
	{"models/earthfighter/sf-22.vw3d", 2.0f, false},
	{"models/earthfighter/weapon1.vw3d", 2.0f, false},
	{"models/earthfighter/weapon2.vw3d", 2.0f, false},
	{"models/earthfighter/weapon3.vw3d", 2.0f, false},
	{"models/earthfighter/weapon4.vw3d", 2.0f, false},
	{"models/earthfighter/weapon5.vw3d", 2.0f, false},
	{"models/earthfighter/weapon6.vw3d", 2.0f, false},
	{"models/earthfighter/weapon7.vw3d", 2.0f, false},
	{"models/earthfighter/weapon8.vw3d", 2.0f, false},
	{"models/earthfighter/weapon9.vw3d", 2.0f, false},
	{"models/earthfighter/weapon10.vw3d", 2.0f, false},
	{"models/earthfighter/weapon11.vw3d", 2.0f, false},
	{"models/earthfighter/weapon12.vw3d", 2.0f, false},
	{"models/earthfighter/weapon13.vw3d", 2.0f, false},
	{"models/earthfighter/weapon14.vw3d", 2.0f, false},
	{"models/earthfighter/weapon15.vw3d", 2.0f, false},
	{"models/earthfighter/lnch1.vw3d", 2.0f, false},
	{"models/earthfighter/lnch2.vw3d", 2.0f, false},
	{"models/earthfighter/lnch3.vw3d", 2.0f, false},
	{"models/earthfighter/lnch4.vw3d", 2.0f, false},
	{"models/earthfighter/missile.vw3d", 2.0f, false},
	{"models/earthfighter/swarm.vw3d", 2.0f, false},
	{"models/earthfighter/torpedo.vw3d", 2.0f, false},
	{"models/earthfighter/nuke.vw3d", 2.0f, false},
	{"models/alienfighter/al-01.vw3d", 2.0f, false},
	{"models/alienfighter/al-02.vw3d", 2.0f, false},
	{"models/alienfighter/al-03.vw3d", 2.0f, false},
	{"models/alienfighter/al-04.vw3d", 2.0f, false},
	{"models/alienfighter/al-05.vw3d", 2.0f, false},
	{"models/alienfighter/al-06.vw3d", 2.0f, false},
	{"models/alienfighter/al-07.vw3d", 2.0f, false},
	{"models/alienfighter/al-08.vw3d", 2.0f, false},
	{"models/alienfighter/al-09.vw3d", 2.0f, false},
	{"models/alienfighter/al-10.vw3d", 2.0f, false},
	{"models/alienfighter/al-11.vw3d", 2.0f, false},
	{"models/alienfighter/al-12.vw3d", 2.0f, false},
	{"models/alienfighter/al-13.vw3d", 2.0f, false},
	{"models/alienfighter/al-14.vw3d", 2.0f, false},
	{"models/alienfighter/al-15.vw3d", 2.0f, false},
	{"models/alienfighter/al-16.vw3d", 2.0f, false},
	{"models/alienfighter/al-17.vw3d", 2.0f, false},
	{"models/pirateship/gunship-01.vw3d", 2.0f, false},
	{"models/pirateship/bomber-07.vw3d", 2.0f, false},
	{"models/pirateship/gunship-03.vw3d", 2.0f, false},
	{"models/pirateship/gunship-04.vw3d", 2.0f, false},
	{"models/pirateship/gunship-02.vw3d", 2.0f, false},
	{"models/pirateship/bomber-03.vw3d", 2.0f, false},
	{"models/pirateship/bomber-02.vw3d", 2.0f, false},
	{"models/pirateship/bomber-04.vw3d", 2.0f, false},
	{"models/pirateship/bomber-05.vw3d", 2.0f, true},
	{"models/pirateship/bomber-06.vw3d", 2.0f, true},
	{"models/turret/turret-01.vw3d", 2.0f, false},
	{"models/turret/turret-02.vw3d", 2.0f, false},
	{"models/tracked/engineering-01.vw3d", 2.0f, false},
	{"models/tracked/apc-aa-02.vw3d", 2.0f, false},
	{"models/tracked/apc-aa-01.vw3d", 2.0f, false},
	{"models/tracked/apc-03.vw3d", 2.0f, false},
	{"models/tracked/apc-01.vw3d", 2.0f, false},
	{"models/tracked/tank-11.vw3d", 2.0f, false},
	{"models/tracked/tank-10.vw3d", 2.0f, false},
	{"models/tracked/tank-09.vw3d", 2.0f, false},
	{"models/tracked/tank-08.vw3d", 2.0f, false},
	{"models/tracked/tank-07.vw3d", 2.0f, false},
	{"models/tracked/tank-06.vw3d", 2.0f, false},
	{"models/tracked/tank-05.vw3d", 2.0f, false},
	{"models/tracked/tank-03.vw3d", 2.0f, false},
	{"models/tracked/tank-01.vw3d", 2.0f, false},
	{"models/wheeled/r-launcher-01.vw3d", 2.0f, false},
	{"models/wheeled/apc-04.vw3d", 2.0f, false},
	{"models/wheeled/apc-02.vw3d", 2.0f, false},
	{"models/wheeled/jeep-05.vw3d", 2.0f, false},
	{"models/wheeled/jeep-04.vw3d", 2.0f, false},
	{"models/wheeled/jeep-03.vw3d", 2.0f, false},
	{"models/wheeled/jeep-02.vw3d", 2.0f, false},
	{"models/wheeled/jeep-01.vw3d", 2.0f, false},
	{"models/militarybuilding/artiler-gun-02.vw3d", 2.0f, false},
	{"models/militarybuilding/artiler-gun-01.vw3d", 2.0f, false},
	{"models/militarybuilding/aa-gun-05.vw3d", 2.0f, false},
	{"models/militarybuilding/aa-gun-04.vw3d", 2.0f, false},
	{"models/militarybuilding/aa-gun-03.vw3d", 2.0f, false},
	{"models/militarybuilding/aa-gun-02.vw3d", 2.0f, false},
	{"models/militarybuilding/aa-gun-01.vw3d", 2.0f, false},
	{"models/spacebase/1/1.vw3d", -1.0f, true},
	{"models/spacebase/1/2.vw3d", -1.0f, true},
	{"models/spacebase/1/3.vw3d", -1.0f, true},
	{"models/spacebase/1/4.vw3d", -1.0f, true},
	{"models/spacebase/1/5.vw3d", -1.0f, true},
	{"models/spacebase/2/1.vw3d", -1.0f, true},
	{"models/spacebase/2/2.vw3d", -1.0f, true},
	{"models/spacebase/2/3.vw3d", -1.0f, true},
	{"models/spacebase/2/4.vw3d", -1.0f, true},
	{"models/spacebase/2/5.vw3d", -1.0f, true},
	{"models/spacebase/3/1.vw3d", -1.0f, false},
	{"models/spacebase/4/1.vw3d", -1.0f, false},
	{"models/spacebase/5/1.vw3d", -1.0f, false},
	{"models/spacebase/6/1.vw3d", -1.0f, true},
	{"models/spacebase/6/2.vw3d", -1.0f, true},
	{"models/spacebase/6/3.vw3d", -1.0f, true},
	{"models/spacebase/6/4.vw3d", -1.0f, true},
	{"models/spacebase/6/5.vw3d", -1.0f, true},
	{"models/spacebase/7/1.vw3d", -1.0f, false},
	{"models/spacebase/8/1.vw3d", -1.0f, true},
	{"models/spacebase/8/2.vw3d", -1.0f, true},
	{"models/spacebase/8/3.vw3d", -1.0f, true},
	{"models/spacebase/8/4.vw3d", -1.0f, true},
	{"models/spacebase/8/5.vw3d", -1.0f, true},
	{"models/space/bigasteroid-01.vw3d", -1.0f, true},
	{"models/space/bigasteroid-02.vw3d", -1.0f, true},
	{"models/space/bigasteroid-03.vw3d", -1.0f, true},
	{"models/space/bigasteroid-04.vw3d", -1.0f, true},
	{"models/space/bigasteroid-05.vw3d", -1.0f, true},
	{"models/space/asteroid-010.vw3d", -1.0f, false},
	{"models/space/asteroid-011.vw3d", -1.0f, false},
	{"models/space/asteroid-012.vw3d", -1.0f, false},
	{"models/space/asteroid-013.vw3d", -1.0f, false},
	{"models/space/asteroid-014.vw3d", -1.0f, false},
	{"models/space/asteroid-015.vw3d", -1.0f, false},
	{"models/space/asteroid-016.vw3d", -1.0f, false},
	{"models/space/asteroid-017.vw3d", -1.0f, false},
	{"models/space/asteroid-018.vw3d", -1.0f, false},
	{"models/space/asteroid-019.vw3d", -1.0f, false},
	{"models/space/asteroid-0110.vw3d", -1.0f, false},
	{"models/space/asteroid-0111.vw3d", -1.0f, false},
	{"models/space/asteroid-0112.vw3d", -1.0f, false},
	{"models/space/asteroid-0113.vw3d", -1.0f, false},
	{"models/space/asteroid-0114.vw3d", -1.0f, false},
	{"models/space/asteroid-0115.vw3d", -1.0f, false},
	{"models/space/asteroid-0116.vw3d", -1.0f, false},
	{"models/space/asteroid-0117.vw3d", -1.0f, false},
	{"models/space/asteroid-0118.vw3d", -1.0f, false},
	{"models/space/asteroid-0119.vw3d", -1.0f, false},
	{"models/planet/aplanet.vw3d", -1.0f, true},
	{"models/planet/dplanet.vw3d", -1.0f, true},
	{"models/planet/gplanet.vw3d", -1.0f, true},
	{"models/planet/moon.vw3d", -1.0f, true},
	{"models/planet/planet5.vw3d", -1.0f, true},
	{"models/planet/planet6.vw3d", -1.0f, true},
	{"models/alienmothership/alm-01.vw3d", 2.0f, true},
	{"models/alienmothership/alm-02.vw3d", 2.0f, true},
	{"models/alienmothership/alm-03.vw3d", 2.0f, true},
	{"models/alienmothership/alm-04.vw3d", 2.0f, true},
	{"models/alienmothership/alm-05.vw3d", 2.0f, true},
	{"models/alienmothership/alm-06.vw3d", 2.0f, true},
	{"models/alienmothership/alm-07.vw3d", 2.0f, true},
	{"models/alienmothership/alm-08.vw3d", 2.0f, true},
	{"models/building/bld-01.vw3d", -1.0f, true},
	{"models/building/bld-02.vw3d", -1.0f, true},
	{"models/building/bld-03.vw3d", -1.0f, true},
	{"models/building/bld-04.vw3d", -1.0f, true},
	{"models/building/bld-05.vw3d", -1.0f, true},
	{"models/building/bld-06.vw3d", -1.0f, true},
	{"models/building/bld-07.vw3d", -1.0f, true},
	{"models/building/bld-08.vw3d", -1.0f, true},
	{"models/building/bld-09.vw3d", -1.0f, true},
	{"models/building/bld-10.vw3d", -1.0f, true},
	{"models/building/bld-11.vw3d", -1.0f, true},
};

} // unnamed namespace


/*
 * Get all model3d assets value.
 */
unsigned GetModel3DAssetsValue()
{
	return Model3DAssetArray.size() * Model3DLoadValue;
}

/*
 * Cycle with function call on each model3d asset load.
 */
void ForEachModel3DAssetLoad(std::function<void (unsigned AssetValue)> function)
{
	for (auto &tmpAsset : Model3DAssetArray) {
		vw_LoadModel3D(tmpAsset.FileName,
			       tmpAsset.TriangleSizeLimit,
			       tmpAsset.NeedTangentAndBinormal && GameConfig().UseGLSL120);
		function(Model3DLoadValue);
	}
}
