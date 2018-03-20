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

#include "../game.h"




float CreditsCurrentPos = 0.0f;
float LastCreditsCurrentPosUpdateTime = 0.0f;




void CreditsMenu()
{

	// делаем движение
	CreditsCurrentPos -= 40.0f*(vw_GetTimeThread(0) - LastCreditsCurrentPosUpdateTime);
	LastCreditsCurrentPosUpdateTime = vw_GetTimeThread(0);

	// зацикливание
	if (CreditsCurrentPos <= -3435) CreditsCurrentPos = 0.0f;




	sRECT SrcRect, DstRect;


	int Y1 = 690+(int)CreditsCurrentPos;
	int Y2 = 690+25+(int)CreditsCurrentPos;
	int Offset1 = 25;
	int Offset2 = 90;


	float ttt = 0.0f;

	int SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_PROJECT_LEADER")))/2;
	if (Y1 > 300) {
		ttt = (650 - Y1) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y1 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, ttt*MenuContentTransp, vw_GetText("10_PROJECT_LEADER"));
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Mikhail_Kurinnoi")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Mikhail_Kurinnoi"));



	Y1 += Offset2;
	Y2 += Offset2;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_2D_ARTWORK")))/2;
	if (Y1 > 300) {
		ttt = (650 - Y1) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y1 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, ttt*MenuContentTransp, vw_GetText("10_2D_ARTWORK"));
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Tatiana_Savitskaya")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Tatiana_Savitskaya"));


	Y1 += Offset2;
	Y2 += Offset2;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_3D_MODELS")))/2;
	if (Y1 > 300) {
		ttt = (650 - Y1) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y1 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, ttt*MenuContentTransp, vw_GetText("10_3D_MODELS"));
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Max_Shelekhov")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Max_Shelekhov"));

	Y1 += Offset1;
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Oleg_Linkov")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Oleg_Linkov"));
	Y1 += Offset1;
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Egor_Kurteanu")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Egor_Kurteanu"));




	Y1 += Offset2;
	Y2 += Offset2;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_PLANETARY_TEXTURES")))/2;
	if (Y1 > 300) {
		ttt = (650 - Y1) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y1 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, ttt*MenuContentTransp, vw_GetText("10_PLANETARY_TEXTURES"));
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Alex_(Esgaroth)_Reuter")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Alex_(Esgaroth)_Reuter"));
	Y1 += Offset1;
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Robert_Stein")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Robert_Stein"));





	Y1 += Offset2;
	Y2 += Offset2;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_MUSIC")))/2;
	if (Y1 > 300) {
		ttt = (650 - Y1) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y1 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, ttt*MenuContentTransp, vw_GetText("10_MUSIC"));
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Alexander_Matchugovsky")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Alexander_Matchugovsky"));


	Y1 += Offset2;
	Y2 += Offset2;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_SOUND_EFFECTS")))/2;
	if (Y1 > 300) {
		ttt = (650 - Y1) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y1 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, ttt*MenuContentTransp, vw_GetText("10_SOUND_EFFECTS"));
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Mikhail_Sirotkin")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Mikhail_Sirotkin"));
	Y1 += Offset1;
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Alexander_Matchugovsky")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Alexander_Matchugovsky"));
	Y1 += Offset1;
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Alexander_Vassilyevsky")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Alexander_Vassilyevsky"));


	Y1 += Offset2;
	Y2 += Offset2;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_VOICE")))/2;
	if (Y1 > 300) {
		ttt = (650 - Y1) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y1 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, ttt*MenuContentTransp, vw_GetText("10_VOICE"));
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Petra_Schulten_(de)")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Petra_Schulten_(de)"));
	Y1 += Offset1;
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Elena_Zavalishina_(en,_ru)")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Elena_Zavalishina_(en,_ru)"));







	Y1 += Offset2;
	Y2 += Offset2;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_WEB_PROGRAMMING")))/2;
	if (Y1 > 300) {
		ttt = (650 - Y1) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y1 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, ttt*MenuContentTransp, vw_GetText("10_WEB_PROGRAMMING"));
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Pavel_Voylov")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Pavel_Voylov"));



	Y1 += Offset2;
	Y2 += Offset2;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_TRANSLATORS")))/2;
	if (Y1 > 300) {
		ttt = (650 - Y1) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y1 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, ttt*MenuContentTransp, vw_GetText("10_TRANSLATORS"));
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Bernd_Dau_(de)")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Bernd_Dau_(de)"));
	Y1 += Offset1;
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Mikhail_Kurinnoi_(en,_ru)")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Mikhail_Kurinnoi_(en,_ru)"));
	Y1 += Offset1;
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Jacek_Fiedorowicz_(pl)")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Jacek_Fiedorowicz_(pl)"));



	Y1 += Offset2;
	Y2 += Offset2;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_TESTERS")))/2;
	if (Y1 > 300) {
		ttt = (650 - Y1) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y1 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, ttt*MenuContentTransp, vw_GetText("10_TESTERS"));
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Dmitriy_Schigolev")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Dmitriy_Schigolev"));
	Y1 += Offset1;
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Ivan_Bobrov")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Ivan_Bobrov"));
	Y1 += Offset1;
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Vadim_Sosenko")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Vadim_Sosenko"));
	Y1 += Offset1;
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Dmitriy_Tatarinov")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Dmitriy_Tatarinov"));
	Y1 += Offset1;
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Alexey_Mosesov")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Alexey_Mosesov"));
	Y1 += Offset1;
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Bernd_Dau")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Bernd_Dau"));
	Y1 += Offset1;
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Ryan_Foster")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Ryan_Foster"));
	Y1 += Offset1;
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Jason_Crowell")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Jason_Crowell"));


	Y1 += Offset2;
	Y2 += Offset2;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_SPECIAL_THANKS_TO")))/2;
	if (Y1 > 300) {
		ttt = (650 - Y1) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y1 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, ttt*MenuContentTransp, vw_GetText("10_SPECIAL_THANKS_TO"));
	// система частиц + хорошие объяснения позволили быстро сделать свою
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Richard_Benson")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Richard_Benson"));
	Y1 += Offset1;
	Y2 += Offset1;
	// помогал с железом
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Alexey_Madalinskiy")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Alexey_Madalinskiy"));
	Y1 += Offset1;
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Olga_Zhuravleva")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Olga_Zhuravleva"));
	Y1 += Offset1;
	Y2 += Offset1;
	// выбор названия игры
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Nick_OConnell")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Nick_OConnell"));
	Y1 += Offset1;
	Y2 += Offset1;
	// помог с mac os x версией
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Mikhail_Storcheous")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Mikhail_Storcheous"));
	Y1 += Offset1;
	Y2 += Offset1;
	// помог разобраться с лицензиями, дал идеи по использованию XDG_CONFIG_HOME и fontconfig
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_Boris_Pek")))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, vw_GetText("10_Boris_Pek"));



#ifndef fontconfig
	Y1 += Offset2;
	Y2 += Offset2;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize("TTF FONTS"))/2;
	if (Y1 > 300) {
		ttt = (650 - Y1) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y1 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, ttt*MenuContentTransp, "TTF FONTS");

	Y1 += Offset1;
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize("Linux Libertine (http://libertine-fonts.org)"))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "Linux Libertine (http://libertine-fonts.org)");

	Y1 += Offset1+15;
	Y2 += Offset1+15;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize("GNU FreeFont (https://www.gnu.org/software/freefont)"))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "GNU FreeFont (https://www.gnu.org/software/freefont)");

	Y1 += Offset1+15;
	Y2 += Offset1+15;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize("Liberation (https://pagure.io/liberation-fonts)"))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "Liberation (https://pagure.io/liberation-fonts)");
#endif // fontconfig




	Y1 += Offset2+10;
	Y2 += Offset2+10;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_POWERED_BY")))/2;
	if (Y1 > 300) {
		ttt = (650 - Y1) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y1 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, ttt*MenuContentTransp, vw_GetText("10_POWERED_BY"));

	int LogoX = Setup.iAspectRatioWidth/2 - 222;
	int LogoTextX = Setup.iAspectRatioWidth/2 - 72;

	// SDL
	Y1 += Offset1-20;
	Y2 += Offset1-20;
	SrcRect(0,0,128,64);
	DstRect(LogoX,Y2,LogoX+128,Y2+64);
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("credits/sdl.tga"), true, ttt*MenuContentTransp);

	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "Simple DirectMedia Layer");
	Y1 += Offset1;
	Y2 += Offset1;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "https://www.libsdl.org");


	// OpenGL
	Y1 += Offset1+20;
	Y2 += Offset1+20;
	SrcRect(0,0,128,64);
	DstRect(LogoX,Y2,LogoX+128,Y2+64);
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("credits/opengl.tga"), true, ttt*MenuContentTransp);

	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "Open Graphics Library");
	Y1 += Offset1;
	Y2 += Offset1;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "https://www.opengl.org");


	// OpenAL
	Y1 += Offset1+20;
	Y2 += Offset1+20;
	SrcRect(0,0,128,64);
	DstRect(LogoX,Y2,LogoX+128,Y2+64);
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("credits/openal.tga"), true, ttt*MenuContentTransp);

	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "Open Audio Library");
	Y1 += Offset1;
	Y2 += Offset1;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "https://www.openal.org");



	// FreeType
	Y1 += Offset1+20;
	Y2 += Offset1+20;
	SrcRect(0,0,128,64);
	DstRect(LogoX,Y2,LogoX+128,Y2+64);
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("credits/freetype.tga"), true, ttt*MenuContentTransp);

	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "FreeType2");
	Y1 += Offset1;
	Y2 += Offset1;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "https://www.freetype.org");


	// ogg vorbis
	Y1 += Offset1+20;
	Y2 += Offset1+20;
	SrcRect(0,0,64,64);
	DstRect(LogoX+32,Y2,LogoX+32+64,Y2+64);
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("credits/oggvorbis.tga"), true, ttt*MenuContentTransp);

	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "Ogg Vorbis");
	Y1 += Offset1;
	Y2 += Offset1;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "https://xiph.org/vorbis");


#ifdef fontconfig
	// fontconfig
	Y1 += Offset1+25;
	Y2 += Offset1+25;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize("Fontconfig"))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "Fontconfig");
	Y1 += Offset1;
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize("http://www.fontconfig.org"))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "http://www.fontconfig.org");
#endif // fontconfig





	Y1 += Offset2+10;
	Y2 += Offset2+10;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("10_DEVELOPING_WITH")))/2;
	if (Y1 > 300) {
		ttt = (650 - Y1) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y1 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, ttt*MenuContentTransp, vw_GetText("10_DEVELOPING_WITH"));

	// Qt Creator
	Y2 += Offset1-20;
	SrcRect(0,0,64,64);
	DstRect(LogoX+32,Y2,LogoX+32+64,Y2+64);
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("credits/qtcreator.tga"), true, ttt*MenuContentTransp);

	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "Qt Creator IDE");
	Y2 += Offset1;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "https://www.qt.io");

	// gcc
	Y2 += Offset1+20;
	SrcRect(0,0,64,64);
	DstRect(LogoX+32,Y2,LogoX+32+64,Y2+64);
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("credits/gnugcc.tga"), true, ttt*MenuContentTransp);

	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "GNU GCC Compiler Collection");
	Y2 += Offset1;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "https://gcc.gnu.org");

	// clang
	Y2 += Offset1+20;
	SrcRect(0,0,64,64);
	DstRect(LogoX+32,Y2,LogoX+32+64,Y2+64);
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("credits/clang.tga"), true, ttt*MenuContentTransp);

	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "Clang, frontend for LLVM");
	Y2 += Offset1;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "http://clang.llvm.org");

	// git
	Y2 += Offset1+20;
	SrcRect(0,0,64,64);
	DstRect(LogoX+32,Y2,LogoX+32+64,Y2+64);
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("credits/git.tga"), true, ttt*MenuContentTransp);

	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "Git");
	Y2 += Offset1;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "https://git-scm.com");


	// gentoo
	Y2 += Offset1+20;
	SrcRect(0,0,64,64);
	DstRect(LogoX+32,Y2,LogoX+32+64,Y2+64);
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("credits/gentoo.tga"), true, ttt*MenuContentTransp);

	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "Gentoo Linux");
	Y2 += Offset1;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "https://www.gentoo.org");


	// gimp
	Y2 += Offset1+20;
	SrcRect(0,0,64,64);
	DstRect(LogoX+32,Y2,LogoX+32+64,Y2+64);
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("credits/gimp.tga"), true, ttt*MenuContentTransp);

	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "GIMP ");
	Y2 += Offset1;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(LogoTextX, Y2+5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "https://www.gimp.org");



	// GIMP normalmap plugin
	Y2 += Offset1+25;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize("GIMP normalmap plugin"))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "GIMP normalmap plugin");
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize("http://code.google.com/p/gimp-normalmap"))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "http://code.google.com/p/gimp-normalmap");



	// Spacescape
	Y2 += Offset1+20;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize("Spacescape"))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "Spacescape");
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize("https://sourceforge.net/projects/spacescape"))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "https://sourceforge.net/projects/spacescape");





	// логотип и копирайт
	Y2 += Offset2+30;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize("Copyright © 2007-2018, Viewizard"))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "Copyright © 2007-2018, Viewizard");
	Y2 += Offset1;
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize("http://www.viewizard.com"))/2;
	if (Y2 > 300) {
		ttt = (650 - Y2) /100.0f;
		if (ttt > 1.0f) ttt = 1.0f;
	} else {
		ttt = (Y2 - 100) /100.0f;
		if (ttt < 0.0f) ttt = 0.0f;
	}
	vw_DrawFont(SizeI, Y2, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ttt*MenuContentTransp, "http://www.viewizard.com");






	int	X = (Setup.iAspectRatioWidth - 384)/2;
	int Y = 165+100*5;
	if (DrawButton384(X,Y, vw_GetText("1_MAIN_MENU"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime))
		ComBuffer = eCommand::SWITCH_TO_MAIN_MENU;

}
