/********************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2012 Michael Kurinnoy, Viewizard


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


	Web Site:		http://www.viewizard.com/
	E-mail:			viewizard@viewizard.com
	Postal address:	kvartal Zhukova, 10/79, Lugansk city, 91051, Ukraine


*********************************************************************************/


#include "Game.h"


const int	ConvertListCount = 388;
const char *ConvertList[ConvertListCount] =
{"DATA_RU/VOICE/EngineMalfunction.wav",
"DATA_RU/VOICE/WeaponDamaged.wav",
"DATA_RU/VOICE/MissileDetected.wav",
"DATA_RU/VOICE/PrepareForAction.wav",
"DATA_RU/VOICE/Attention.wav",
"DATA_RU/VOICE/WeaponDestroyed.wav",
"DATA_RU/VOICE/WeaponMalfunction.wav",
"DATA_RU/VOICE/CeaseFire.wav",
"DATA_RU/VOICE/PowerSupplyReestablished.wav",
"DATA_RU/VOICE/ReactorMalfunction.wav",
"DATA_RU/VOICE/Warning.wav",
"DATA_RU/MENU/button_weaponry_in.tga",
"DATA_RU/MENU/button_weaponry_out.tga",
"DATA_RU/GAME/pause.tga",
"DATA_RU/GAME/mission.tga",
"DATA_RU/GAME/missionfailed.tga",
"LOADING/loading_back.tga",
"MENU/weapon_empty_icon.tga",
"MENU/system_engine1.tga",
"MENU/system_engine2.tga",
"MENU/system_engine3.tga",
"MENU/system_engine4.tga",
"MENU/system_target1.tga",
"MENU/system_target2.tga",
"MENU/system_target3.tga",
"MENU/system_target4.tga",
"MENU/system_power1.tga",
"MENU/system_power2.tga",
"MENU/system_power3.tga",
"MENU/system_power4.tga",
"MENU/system_protect1.tga",
"MENU/system_protect2.tga",
"MENU/system_protect3.tga",
"MENU/system_protect4.tga",
"MENU/system_mechan1.tga",
"MENU/system_mechan2.tga",
"MENU/system_mechan3.tga",
"MENU/system_mechan4.tga",
"MENU/system_empty.tga",
"GLSL/light.vert",
"GLSL/light.frag",
"GLSL/light_shadowmap.vert",
"GLSL/light_shadowmap.frag",
"GLSL/particle.frag",
"GLSL/particle_stars.vert",
"GLSL/light_explosion.frag",
"GLSL/light_explosion.vert",
"GLSL/particle.vert",
"CREDITS/gentoo.tga",
"CREDITS/gimp.tga",
"CREDITS/sdl.tga",
"CREDITS/opengl.tga",
"CREDITS/gnugcc.tga",
"CREDITS/tinyxml.tga",
"CREDITS/codeblocks.tga",
"CREDITS/openal.tga",
"CREDITS/svn.tga",
"MENU/cursor.tga",
"DATA_EN/VOICE/EngineMalfunction.wav",
"DATA_EN/VOICE/WeaponDamaged.wav",
"DATA_EN/VOICE/MissileDetected.wav",
"DATA_EN/VOICE/PrepareForAction.wav",
"DATA_EN/VOICE/Attention.wav",
"DATA_EN/VOICE/WeaponDestroyed.wav",
"DATA_EN/VOICE/WeaponMalfunction.wav",
"DATA_EN/VOICE/CeaseFire.wav",
"DATA_EN/VOICE/PowerSupplyReestablished.wav",
"DATA_EN/VOICE/ReactorMalfunction.wav",
"DATA_EN/VOICE/Warning.wav",
"DATA_EN/MENU/button_weaponry_in.tga",
"DATA_EN/MENU/button_weaponry_out.tga",
"DATA_EN/GAME/pause.tga",
"DATA_EN/GAME/mission.tga",
"DATA_EN/GAME/missionfailed.tga",
"SFX/weaponfire12.wav",
"SFX/drag_offslot.wav",
"SFX/menu_click.wav",
"SFX/drag_error.wav",
"SFX/menu_onbutton.wav",
"SFX/weapon1probl.wav",
"SFX/weaponfire3.wav",
"SFX/gausshit.wav",
"SFX/menu_online.wav",
"SFX/weaponfire5.wav",
"SFX/weapon4probl.wav",
"SFX/drag_release.wav",
"SFX/menu_selectline.wav",
"SFX/weaponfire1.wav",
"SFX/weaponfire14.wav",
"SFX/weaponfire13.wav",
"SFX/weaponfire16.wav",
"SFX/weaponfire17.wav",
"SFX/explosion4.wav",
"SFX/weapon3probl.wav",
"SFX/plasmahit.wav",
"SFX/weaponfire19.wav",
"SFX/menu_nclick.wav",
"SFX/drag_onslot.wav",
"SFX/weaponfire10.wav",
"SFX/ionhit.wav",
"SFX/weaponfire6.wav",
"SFX/weaponfire7.wav",
"SFX/weaponfire15.wav",
"SFX/explosion2.wav",
"SFX/weaponfire8.wav",
"SFX/weaponfire2.wav",
"SFX/menu_onbutton2.wav",
"SFX/weaponfire18.wav",
"SFX/menu_new.wav",
"SFX/weaponfire4.wav",
"SFX/antimaterhit.wav",
"SFX/weapon5probl.wav",
"SFX/explosion3.wav",
"SFX/kinetichit.wav",
"SFX/weaponfire9.wav",
"SFX/menu_taping.wav",
"SFX/game_showmenu.wav",
"SFX/game_hidemenu.wav",
"SFX/weaponfire11.wav",
"SFX/weapon2probl.wav",
"SFX/explosion1.wav",
"SFX/lowlife.wav",
"MODELS/MINE/mine2i.tga",
"MODELS/MINE/mine1.tga",
"MODELS/MINE/mine3i.tga",
"MODELS/MINE/mine3.tga",
"MODELS/MINE/mine-04.VW3D",
"MODELS/MINE/mine-01.VW3D",
"MODELS/MINE/mine2.tga",
"MODELS/MINE/mine-03.VW3D",
"MODELS/MINE/mine1i.tga",
"MODELS/MINE/mine-02.VW3D",
"MODELS/MINE/mine4i.tga",
"MODELS/MINE/mine4.tga",
"MODELS/PLANET/planet5.VW3D",
"MODELS/PLANET/aplanet.VW3D",
"MODELS/PLANET/dplanet.VW3D",
"MODELS/PLANET/m_class7.tga",
"MODELS/PLANET/a_class4.tga",
"MODELS/PLANET/d_class3.tga",
"MODELS/PLANET/planet6.VW3D",
"MODELS/PLANET/q_class2.tga",
"MODELS/PLANET/gplanet.VW3D",
"MODELS/PLANET/moon.VW3D",
"MODELS/PLANET/clouds.tga",
"MODELS/PLANET/asteroid.tga",
"MODELS/SPACEBASE/4/1.VW3D",
"MODELS/SPACEBASE/3/1.VW3D",
"MODELS/SPACEBASE/7/1.VW3D",
"MODELS/SPACEBASE/1/2.VW3D",
"MODELS/SPACEBASE/1/4.VW3D",
"MODELS/SPACEBASE/1/3.VW3D",
"MODELS/SPACEBASE/1/1.VW3D",
"MODELS/SPACEBASE/1/5.VW3D",
"MODELS/SPACEBASE/metal.tga",
"MODELS/SPACEBASE/5/1.VW3D",
"MODELS/SPACEBASE/6/2.VW3D",
"MODELS/SPACEBASE/6/4.VW3D",
"MODELS/SPACEBASE/6/3.VW3D",
"MODELS/SPACEBASE/6/1.VW3D",
"MODELS/SPACEBASE/6/5.VW3D",
"MODELS/SPACEBASE/8/2.VW3D",
"MODELS/SPACEBASE/8/4.VW3D",
"MODELS/SPACEBASE/8/3.VW3D",
"MODELS/SPACEBASE/8/1.VW3D",
"MODELS/SPACEBASE/8/5.VW3D",
"MODELS/SPACEBASE/allalpha.tga",
"MODELS/SPACEBASE/2/2.VW3D",
"MODELS/SPACEBASE/2/4.VW3D",
"MODELS/SPACEBASE/2/3.VW3D",
"MODELS/SPACEBASE/2/1.VW3D",
"MODELS/SPACEBASE/2/5.VW3D",
"MODELS/TURRET/turret-02.VW3D",
"MODELS/TURRET/turret-01.VW3D",
"MODELS/TURRET/turrets.tga",
"MODELS/SPACE/asteroid-0118.VW3D",
"MODELS/SPACE/asteroid-0116.VW3D",
"MODELS/SPACE/asteroid-011.VW3D",
"MODELS/SPACE/bigasteroid-03.VW3D",
"MODELS/SPACE/asteroid-018.VW3D",
"MODELS/SPACE/asteroid-017.VW3D",
"MODELS/SPACE/asteroid-016.VW3D",
"MODELS/SPACE/bigasteroid-02.VW3D",
"MODELS/SPACE/bigasteroid-05.VW3D",
"MODELS/SPACE/asteroid-01.tga",
"MODELS/SPACE/asteroid-0110.VW3D",
"MODELS/SPACE/asteroid-0115.VW3D",
"MODELS/SPACE/asteroid-0112.VW3D",
"MODELS/SPACE/asteroid-0117.VW3D",
"MODELS/SPACE/asteroid-012.VW3D",
"MODELS/SPACE/asteroid-019.VW3D",
"MODELS/SPACE/bigasteroid-01.VW3D",
"MODELS/SPACE/asteroid-013.VW3D",
"MODELS/SPACE/asteroid-0119.VW3D",
"MODELS/SPACE/asteroid-0114.VW3D",
"MODELS/SPACE/asteroid-014.VW3D",
"MODELS/SPACE/asteroid-0113.VW3D",
"MODELS/SPACE/asteroid-015.VW3D",
"MODELS/SPACE/bigasteroid-04.VW3D",
"MODELS/SPACE/asteroid-010.VW3D",
"MODELS/SPACE/asteroid-0111.VW3D",
"MODELS/EARTHFIGHTER/lnch1.VW3D",
"MODELS/EARTHFIGHTER/lnch2.VW3D",
"MODELS/EARTHFIGHTER/lnch3.VW3D",
"MODELS/EARTHFIGHTER/lnch4.VW3D",
"MODELS/EARTHFIGHTER/rockets.tga",
"MODELS/EARTHFIGHTER/torpedo.VW3D",
"MODELS/EARTHFIGHTER/swarm.VW3D",
"MODELS/EARTHFIGHTER/lnch34.tga",
"MODELS/EARTHFIGHTER/missile.VW3D",
"MODELS/EARTHFIGHTER/nuke.VW3D",
"MODELS/EARTHFIGHTER/lnch12.tga",
"MENU/weapon4_icon.tga",
"MENU/ammo.tga",
"MENU/weapon17_icon.tga",
"MENU/workshop_panel1.tga",
"MENU/weapon18_icon.tga",
"MENU/weapon14_icon.tga",
"MENU/weapon19_icon.tga",
"MENU/weapon13_icon.tga",
"MENU/panel800_444_back.tga",
"MENU/button384_back.tga",
"MENU/panel444_333_back.tga",
"MENU/weapon2_icon.tga",
"MENU/button_dialog128_in.tga",
"MENU/weapon6_icon.tga",
"MENU/arrows_off.tga",
"MENU/arrows_blue.tga",
"MENU/workshop_panel5.tga",
"MENU/workshop_panel3.tga",
"MENU/dialog768_600.tga",
"MENU/dialog512_256.tga",
"MENU/perc_none.tga",
"MENU/weapon5_icon.tga",
"MENU/panel444_333_border.tga",
"MENU/button_dialog200_out.tga",
"MENU/workshop_panel4.tga",
"MENU/weapon12_icon.tga",
"MENU/whitepoint.tga",
"MENU/weapon16_icon.tga",
"MENU/weapon11_icon.tga",
"MENU/back_spot2.tga",
"MENU/button256_off.tga",
"MENU/blackpoint.tga",
"MENU/checkbox_main.tga",
"MENU/button_dialog200_off.tga",
"MENU/workshop_panel1+.tga",
"MENU/weapon10_icon.tga",
"MENU/perc.tga",
"MENU/workshop_panel2.tga",
"MENU/dialog512_512.tga",
"MENU/button_dialog200_in.tga",
"MENU/button384_out.tga",
"MENU/button384_in.tga",
"MENU/weapon15_icon.tga",
"MENU/workshop_panel2+.tga",
"MENU/line.tga",
"MENU/button256_out.tga",
"MENU/weapon3_icon.tga",
"MENU/button256_in.tga",
"MENU/back_spot.tga",
"MENU/button256_back.tga",
"MENU/weapon_on_icon.tga",
"MENU/weapon8_icon.tga",
"MENU/arrows_on.tga",
"MENU/astromenace.tga",
"MENU/weapon9_icon.tga",
"MENU/weapon1_icon.tga",
"MENU/checkbox_in.tga",
"MENU/weapon7_icon.tga",
"MENU/button_dialog128_out.tga",
"MENU/button_dialog128_off.tga",
"GAME/ammo.tga",
"GAME/weapon_panel_left.tga",
"GAME/game_panel.tga",
"GAME/weapon_ammo.tga",
"GAME/game_panel2.tga",
"GAME/energy.tga",
"GAME/nums.tga",
"GAME/game_num.tga",
"GAME/weapon_panel_right.tga",
"GAME/game_panel_el.tga",
"GAME/weapon_energy.tga",
"GFX/flare2.tga",
"GFX/flare.tga",
"GFX/flare1.tga",
"GFX/flare3.tga",
"LOADING/viewizardlogo.tga",
"LOADING/loading03.tga",
"LOADING/loading_line.tga",
"LOADING/loading01.tga",
"LOADING/loading02.tga",
"LOADING/loading04.tga",
"DATA_DE/VOICE/EngineMalfunction.wav",
"DATA_DE/VOICE/WeaponDamaged.wav",
"DATA_DE/VOICE/MissileDetected.wav",
"DATA_DE/VOICE/PrepareForAction.wav",
"DATA_DE/VOICE/Attention.wav",
"DATA_DE/VOICE/WeaponDestroyed.wav",
"DATA_DE/VOICE/WeaponMalfunction.wav",
"DATA_DE/VOICE/CeaseFire.wav",
"DATA_DE/VOICE/PowerSupplyReestablished.wav",
"DATA_DE/VOICE/ReactorMalfunction.wav",
"DATA_DE/VOICE/Warning.wav",
"DATA_DE/MENU/button_weaponry_in.tga",
"DATA_DE/MENU/button_weaponry_out.tga",
"DATA_DE/GAME/pause.tga",
"DATA_DE/GAME/mission.tga",
"DATA_DE/GAME/missionfailed.tga",
"MENU/cursor_shadow.tga",
"MUSIC/boss-intro.ogg",
"MUSIC/menu.ogg",
"MUSIC/boss-loop.ogg",
"MUSIC/game.ogg",
"MUSIC/missionfailed.ogg",
"MUSIC/intro.ogg",
"SCRIPT/mission6.xml",
"SCRIPT/list.xml",
"SCRIPT/mission14.xml",
"SCRIPT/mission8.xml",
"SCRIPT/mission9_icon.tga",
"SCRIPT/mission15_icon.tga",
"SCRIPT/mission3.xml",
"SCRIPT/mission7.xml",
"SCRIPT/mission12.xml",
"SCRIPT/mission5.xml",
"SCRIPT/mission1.xml",
"SCRIPT/mission10_icon.tga",
"SCRIPT/mission13_icon.tga",
"SCRIPT/menu3.xml",
"SCRIPT/mission5_icon.tga",
"SCRIPT/mission13.xml",
"SCRIPT/mission14_icon.tga",
"SCRIPT/mission6_icon.tga",
"SCRIPT/mission3_icon.tga",
"SCRIPT/mission11.xml",
"SCRIPT/mission4.xml",
"SCRIPT/mission2.xml",
"SCRIPT/mission11_icon.tga",
"SCRIPT/mission4_icon.tga",
"SCRIPT/mission8_icon.tga",
"SCRIPT/mission7_icon.tga",
"SCRIPT/aimode.xml",
"SCRIPT/mission10.xml",
"SCRIPT/mission2_icon.tga",
"SCRIPT/mission12_icon.tga",
"SCRIPT/menu2.xml",
"SCRIPT/mission15.xml",
"SCRIPT/mission1_icon.tga",
"SCRIPT/mission9.xml",
"SCRIPT/menu1.xml",
"SKYBOX/1/skybox_front5.tga",
"SKYBOX/1/skybox_left2.tga",
"SKYBOX/1/skybox_right1.tga",
"SKYBOX/1/skybox_back6.tga",
"SKYBOX/1/skybox_bottom4.tga",
"SKYBOX/1/skybox_top3.tga",
"SKYBOX/tile_back.tga",
"SKYBOX/tile_stars.tga",
"SKYBOX/2/skybox_front5.tga",
"SKYBOX/2/skybox_left2.tga",
"SKYBOX/2/skybox_right1.tga",
"SKYBOX/2/skybox_back6.tga",
"SKYBOX/2/skybox_bottom4.tga",
"SKYBOX/2/skybox_top3.tga",
"FONT/LiberationMono-Bold.ttf",
"FONT/LiberationSans-Bold.ttf",
"FONT/LiberationSerif-Bold.ttf",
"FONT/FreeMonoBold.ttf",
"FONT/FreeSansBold.ttf",
"FONT/FreeSerifBold.ttf",
"FONT/Ubuntu-B.ttf",
"CREDITS/freetype.tga",
"CREDITS/oggvorbis.tga",
"text.csv",
"GFX/trail1.tga",
"GFX/trail2.tga",
"GFX/trail3.tga",
"GFX/trail4.tga",
"GFX/trail5.tga",
"MODELS/NORMALMAP/alien_mothership_nm.tga",
"MODELS/NORMALMAP/m_class7_nm.tga",
"MODELS/NORMALMAP/d_class3_nm.tga",
"MODELS/NORMALMAP/a_class4_nm.tga",
"MODELS/NORMALMAP/planet_asteroids_nm.tga",
"MODELS/NORMALMAP/q_class2_nm.tga",
"MODELS/NORMALMAP/bomber_nm.tga",
"MODELS/NORMALMAP/buildings_nm.tga",
};







const int	ModelsPackListCount = 130;
const char *ModelsPackList[ModelsPackListCount] =
{
"MODELS/ALIENFIGHTER/al-04.VW3D",
"MODELS/ALIENFIGHTER/al-06.VW3D",
"MODELS/ALIENFIGHTER/al-12.VW3D",
"MODELS/ALIENFIGHTER/al-10.VW3D",
"MODELS/ALIENFIGHTER/al-16.VW3D",
"MODELS/ALIENFIGHTER/al-illum04.VW2D",
"MODELS/ALIENFIGHTER/al-13.VW3D",
"MODELS/ALIENFIGHTER/al-07.VW3D",
"MODELS/ALIENFIGHTER/al-02.VW3D",
"MODELS/ALIENFIGHTER/al-15.VW3D",
"MODELS/ALIENFIGHTER/al-03.VW3D",
"MODELS/ALIENFIGHTER/al-01.VW3D",
"MODELS/ALIENFIGHTER/al-14.VW3D",
"MODELS/ALIENFIGHTER/al-text04.VW2D",
"MODELS/ALIENFIGHTER/al-08.VW3D",
"MODELS/ALIENFIGHTER/al-11.VW3D",
"MODELS/ALIENFIGHTER/al-17.VW3D",
"MODELS/ALIENFIGHTER/al-05.VW3D",
"MODELS/ALIENFIGHTER/al-09.VW3D",
"MODELS/ALIENMOTHERSHIP/alm-01.VW3D",
"MODELS/ALIENMOTHERSHIP/alm-04.VW3D",
"MODELS/ALIENMOTHERSHIP/alm-text04.VW2D",
"MODELS/ALIENMOTHERSHIP/alm-text03.VW2D",
"MODELS/ALIENMOTHERSHIP/alm-05.VW3D",
"MODELS/ALIENMOTHERSHIP/alm-illum04.VW2D",
"MODELS/ALIENMOTHERSHIP/alm-02.VW3D",
"MODELS/ALIENMOTHERSHIP/alm-03.VW3D",
"MODELS/ALIENMOTHERSHIP/alm-illum08.VW2D",
"MODELS/ALIENMOTHERSHIP/alm-illum03.VW2D",
"MODELS/ALIENMOTHERSHIP/alm-06.VW3D",
"MODELS/ALIENMOTHERSHIP/alm-07.VW3D",
"MODELS/ALIENMOTHERSHIP/alm-text02.VW2D",
"MODELS/ALIENMOTHERSHIP/alm-text08.VW2D",
"MODELS/ALIENMOTHERSHIP/alm-illum02.VW2D",
"MODELS/ALIENMOTHERSHIP/alm-08.VW3D",
"MODELS/BUILDING/bld-08.VW3D",
"MODELS/BUILDING/bld-06.VW3D",
"MODELS/BUILDING/bld-07.VW3D",
"MODELS/BUILDING/bld-02.VW3D",
"MODELS/BUILDING/bld.VW2D",
"MODELS/BUILDING/bld-03.VW3D",
"MODELS/BUILDING/bld-09.VW3D",
"MODELS/BUILDING/bld-04.VW3D",
"MODELS/BUILDING/bld-05.VW3D",
"MODELS/BUILDING/bld_illum.VW2D",
"MODELS/BUILDING/bld-10.VW3D",
"MODELS/BUILDING/bld-01.VW3D",
"MODELS/BUILDING/bld-11.VW3D",
"MODELS/EARTHFIGHTER/sf-07.VW3D",
"MODELS/EARTHFIGHTER/sf-16.VW3D",
"MODELS/EARTHFIGHTER/sf-06.VW3D",
"MODELS/EARTHFIGHTER/sf-09.VW3D",
"MODELS/EARTHFIGHTER/sf-01.VW3D",
"MODELS/EARTHFIGHTER/sf-text05.VW2D",
"MODELS/EARTHFIGHTER/sf-text09.VW2D",
"MODELS/EARTHFIGHTER/sf-19.VW3D",
"MODELS/EARTHFIGHTER/sf-illum01.VW2D",
"MODELS/EARTHFIGHTER/sf-text00.VW2D",
"MODELS/EARTHFIGHTER/sf-text10.VW2D",
"MODELS/EARTHFIGHTER/sf-05.VW3D",
"MODELS/EARTHFIGHTER/sf-14.VW3D",
"MODELS/EARTHFIGHTER/sf-illum02.VW2D",
"MODELS/EARTHFIGHTER/sf-illum03.VW2D",
"MODELS/EARTHFIGHTER/sf-17.VW3D",
"MODELS/EARTHFIGHTER/sf-12.VW3D",
"MODELS/EARTHFIGHTER/sf-13.VW3D",
"MODELS/EARTHFIGHTER/sf-text08.VW2D",
"MODELS/EARTHFIGHTER/sf-03.VW3D",
"MODELS/EARTHFIGHTER/sf-15.VW3D",
"MODELS/EARTHFIGHTER/sf-04.VW3D",
"MODELS/EARTHFIGHTER/sf-text07.VW2D",
"MODELS/EARTHFIGHTER/sf-20.VW3D",
"MODELS/EARTHFIGHTER/sf-10.VW3D",
"MODELS/EARTHFIGHTER/sf-11.VW3D",
"MODELS/EARTHFIGHTER/sf-22.VW3D",
"MODELS/EARTHFIGHTER/sf-21.VW3D",
"MODELS/EARTHFIGHTER/sf-text06.VW2D",
"MODELS/EARTHFIGHTER/sf-18.VW3D",
"MODELS/EARTHFIGHTER/sf-08.VW3D",
"MODELS/EARTHFIGHTER/sf-illum04.VW2D",
"MODELS/EARTHFIGHTER/weapons.VW3D",
"MODELS/EARTHFIGHTER/sf-text04.VW2D",
"MODELS/EARTHFIGHTER/sf-02.VW3D",
"MODELS/MILITARYBUILDING/artiler-gun-01.VW3D",
"MODELS/MILITARYBUILDING/aa-gun-03.VW3D",
"MODELS/MILITARYBUILDING/aa-gun-04.VW3D",
"MODELS/MILITARYBUILDING/aa-gun-05.VW3D",
"MODELS/MILITARYBUILDING/aa-gun-02.VW3D",
"MODELS/MILITARYBUILDING/aa-gun-01.VW3D",
"MODELS/MILITARYBUILDING/artiler-gun-02.VW3D",
"MODELS/PIRATESHIP/bomber-06.VW3D",
"MODELS/PIRATESHIP/gunship-01.VW3D",
"MODELS/PIRATESHIP/gunship-04.VW3D",
"MODELS/PIRATESHIP/gunship-03.VW3D",
"MODELS/PIRATESHIP/gunship-02.VW3D",
"MODELS/PIRATESHIP/bomber-04.VW3D",
"MODELS/PIRATESHIP/bomber-03.VW3D",
"MODELS/PIRATESHIP/bomber-05.VW3D",
"MODELS/PIRATESHIP/bomber-07.VW3D",
"MODELS/PIRATESHIP/bomber-02.VW3D",
"MODELS/TRACKED/apc-03.VW3D",
"MODELS/TRACKED/apc-01.VW3D",
"MODELS/TRACKED/tank-03.VW3D",
"MODELS/TRACKED/tank-09.VW3D",
"MODELS/TRACKED/tank-08.VW3D",
"MODELS/TRACKED/tank-07.VW3D",
"MODELS/TRACKED/tank-05.VW3D",
"MODELS/TRACKED/apc-aa-01.VW3D",
"MODELS/TRACKED/tank-01.VW3D",
"MODELS/TRACKED/engineering-01.VW3D",
"MODELS/TRACKED/tank-10.VW3D",
"MODELS/TRACKED/apc-aa-02.VW3D",
"MODELS/TRACKED/tank-06.VW3D",
"MODELS/TRACKED/tank-11.VW3D",
"MODELS/WHEELED/apc-04.VW3D",
"MODELS/WHEELED/jeep-01.VW3D",
"MODELS/WHEELED/apc-02.VW3D",
"MODELS/WHEELED/jeep-03.VW3D",
"MODELS/WHEELED/jeep-02.VW3D",
"MODELS/WHEELED/jeep-05.VW3D",
"MODELS/WHEELED/jeep-04.VW3D",
"MODELS/WHEELED/r-launcher-01.VW3D",
"MODELS/track.VW2D",
"MODELS/gr-04.VW2D",
"MODELS/gr-07.VW2D",
"MODELS/gr-01.VW2D",
"MODELS/gr-06.VW2D",
"MODELS/gr-03.VW2D",
"MODELS/gr-05.VW2D",
"MODELS/gr-02.VW2D"
};











//------------------------------------------------------------------------------------
// конвертирование данных в VFS файл
//------------------------------------------------------------------------------------
int ConvertFS2VFS(char RawDataDir[MAX_PATH])
{

	char SrcFileName[MAX_PATH];
	char DstFileName[MAX_PATH];


	// часть данных у нас упакованна, и распространяется под лицензией СС
	// ее надо интегрировать в основной файл данных игры

	char ModelsPackFile[MAX_PATH];
	strcpy(ModelsPackFile, RawDataDir);
	strcat(ModelsPackFile, "MODELS/models.pack");

	BYTE *filedata[ModelsPackListCount];
	int filedatasize[ModelsPackListCount];

	if (vw_OpenVFS(ModelsPackFile, 0) != 0)
	{
		fprintf(stderr, "models.pack file not found or corrupted.\n");
		return -1;
	}


	for (int i=0; i<ModelsPackListCount; i++)
	{
		// все наши файлы внутри VFS лежат в директории DATA
		strcpy(DstFileName, "DATA/");
		strcat(DstFileName, ModelsPackList[i]);

		eFILE *tmpFile = vw_fopen(DstFileName);
		if (tmpFile == 0)
		{
			fprintf(stderr, "models.pack file corrupted, model %s not found.\n", DstFileName);
			return -1;
		}

		tmpFile->fseek( 0, SEEK_END );
		filedatasize[i] = tmpFile->ftell();
		tmpFile->fseek( 0, SEEK_SET );

		filedata[i] = new BYTE[filedatasize[i]];
		tmpFile->fread(filedata[i], filedatasize[i], 1);

		vw_fclose(tmpFile);
	}


	vw_ShutdownVFS();







	// создаем новый файл VFS
	vw_CreateVFS(VFSFileNamePath, GAME_BUILD);


	// используем не авто поиск по дирректориям, как было, а заранее сформированный список
	// 1) файлы очень редко удаляются/добавляются
	// 2) нужно обязательно проверить "целостность" всей сборки на наличие всех файлов
	// 3) будет работать на любой платформе, где работает libSDL




	// добавляем файлы из пакета 3д моделей
	for (int i=0; i<ModelsPackListCount; i++)
	{
		strcpy(DstFileName, "DATA/");
		strcat(DstFileName, ModelsPackList[i]);

		// запись в VFS
		if (0 != vw_WriteIntoVFSfromMemory(DstFileName, filedata[i], filedatasize[i]))
		{
			// какая-то ошибка, не можем записать в VFS
			fprintf(stderr, "Can't write into VFS from memory %s !!!\n", DstFileName);
			fprintf(stderr, "VFS compilation process aborted!\n");
			return -1;
		}

		delete [] filedata[i]; filedata[i] = 0;
		filedatasize[i] = 0;
	}




	// добавляем физические файлы
	for (int i=0; i<ConvertListCount; i++)
	{

		strcpy(SrcFileName, RawDataDir);
		strcat(SrcFileName, ConvertList[i]);

		// все наши файлы внутри VFS лежат в директории DATA
		strcpy(DstFileName, "DATA/");
		strcat(DstFileName, ConvertList[i]);


		// читаем данные файла в буфер
		SDL_RWops *Ftmp = SDL_RWFromFile(SrcFileName, "rb");
		// Если файл не найден - выходим
		if (Ftmp == NULL)
    	{
			fprintf(stderr, "Can't find file %s !!!\n", SrcFileName);
			fprintf(stderr, "VFS compilation process aborted!\n");
        	return -1;
    	}

		// получаем размер файла
		SDL_RWseek(Ftmp, 0, SEEK_END);
		int tmpLength = SDL_RWtell(Ftmp);
		SDL_RWseek(Ftmp, 0, SEEK_SET);

		// копируем все данные файла в массив
		BYTE *tmp = 0;
		tmp = new BYTE[tmpLength];
		SDL_RWread(Ftmp, tmp, tmpLength, 1);
		SDL_RWclose(Ftmp);

		// запись в VFS
		if (0 != vw_WriteIntoVFSfromMemory(DstFileName, tmp, tmpLength))
		{
			// какая-то ошибка, не можем записать в VFS
			delete [] tmp; tmp = 0;
			fprintf(stderr, "Can't write into VFS from memory %s !!!\n", DstFileName);
			fprintf(stderr, "VFS compilation process aborted!\n");
			return -1;
		}

		// Освобождаем память
		delete [] tmp; tmp = 0;
	}


	// закрываем VFS файл
	vw_ShutdownVFS();

	return 0;
}




