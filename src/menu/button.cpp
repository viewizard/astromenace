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

#include "../game.h"


extern bool DragWeapon;



// для управление через клавиатуру
int CurrentActiveMenuElement = 0;
int CurrentKeyboardSelectMenuElement = 0;




// для кнопок, чтобы проигрывать наведение
// последние координаты кнопки на которую наводились
int NeedPlayOnButtonSoundX = 0;
int NeedPlayOnButtonSoundY = 0;



//------------------------------------------------------------------------------------
// прорисовка кнопки 384
//------------------------------------------------------------------------------------
bool DrawButton384(int X, int Y, const char *Text, float Transp, float *ButTransp, float *Update)
{
	sRECT SrcRect, DstRect;
	bool ON = false;
	bool CanClick = false;
	float IntTransp = Transp;


	// работаем с клавиатурой
	if ((Transp >= 0.99f) && !isDialogBoxDrawing() && DrawGameCursor) CurrentActiveMenuElement++;
	bool InFocusByKeyboard = false;
	if (CurrentKeyboardSelectMenuElement > 0) {
		if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement) {
			InFocusByKeyboard = true;
		}
	}



	DstRect(X+2,Y+1,X+384,Y+63);
	if  ((vw_MouseOverRect(DstRect) || InFocusByKeyboard) && !isDialogBoxDrawing() && DrawGameCursor) {
		// если тухнем или появляемся - не жать
		ON = true;
		if (Transp==1.0f) {
			CanClick = true;
			CurrentCursorStatus = 1;
		}


		if (*ButTransp == 1.0f) {
			Audio_PlaySound2D(1,1.0f);
			*Update = vw_GetTimeThread(0);
			*ButTransp = 0.99f;
		}

		(*ButTransp) -= 3.0f*(vw_GetTimeThread(0) - (*Update));
		if (*ButTransp < 0.60f) *ButTransp = 0.60f;
		*Update = vw_GetTimeThread(0);

		IntTransp = (*ButTransp) * Transp;
	} else {
		if (*ButTransp < 1.0f) {
			(*ButTransp) += 3.0f*(vw_GetTimeThread(0) - (*Update));
			if (*ButTransp > 1.0f) *ButTransp =1.0f;
			*Update = vw_GetTimeThread(0);
		}

		IntTransp = (*ButTransp) * Transp;
	}



	// размер задней тени
	SrcRect(2,2,512-2,96-2 );
	// рисуем тень
	DstRect(X-64+2,Y-17+2,X-64+512-2,Y-17+96-2);
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/button384_back.tga"), true, IntTransp);

	SrcRect(0,0,384,64 );
	// рисуем кнопку
	DstRect(X,Y,X+384,Y+64);
	if (!ON)
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/button384_out.tga"), true, Transp);
	else
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/button384_in.tga"), true, Transp);



	// получаем длину текста
	int Size = vw_TextWidth(Text);

	// если текст сильно большой - сжимаем буквы, чтобы не вылазило за пределы кнопки
	float WScale = 0;
	if (Size > 310) {
		Size = 310;
		WScale = -310;
	}

	// находим смещение текста
	int SizeI = X + (SrcRect.right-SrcRect.left-Size)/2;

	// рисуем текст
	if (!ON)
		vw_DrawText(SizeI, Y+21, WScale, 0, 1.0f, eRGBCOLOR::white, (0.7f*Transp)/2.0f, Text);
	else
		vw_DrawText(SizeI, Y+21, WScale, 0, 1.0f, eRGBCOLOR::white, Transp, Text);



	if (CanClick)
		if (vw_GetMouseLeftClick(true) ||
		    (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN)))) {
			Audio_PlaySound2D(2,1.0f);
			if (InFocusByKeyboard) {
				vw_SetKeyStatus(SDLK_KP_ENTER, false);
				vw_SetKeyStatus(SDLK_RETURN, false);
			}
			return true;
		}

	return false;
}










//------------------------------------------------------------------------------------
// прорисовка кнопки - 256
//------------------------------------------------------------------------------------
bool DrawButton256(int X, int Y, const char *Text, float Transp, float *ButTransp, float *Update, bool Off)
{
	sRECT SrcRect, DstRect;


	if (Off || DragWeapon) {

		SrcRect(2,2,512-2,96-2 );
		DstRect(X-125+2,Y-16+2,X-125+512-2,Y-16+96-2);
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/button256_back.tga"), true, Transp);

		SrcRect(0,0,256,64 );
		DstRect(X,Y,X+256,Y+64);
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/button256_off.tga"), true, Transp);


		int Size = vw_TextWidth(Text);
		int SizeI = DstRect.left + (SrcRect.right-SrcRect.left-Size)/2;
		vw_DrawText(SizeI, Y+21, 0, 0, 1.0f, eRGBCOLOR::white, (0.7f*Transp)/2.0f, Text);

		DstRect(X+2,Y+1,X+256,Y+63);
		if  (vw_MouseOverRect(DstRect) && !isDialogBoxDrawing()) {
			if (Transp==1.0f && !DragWeapon) {
				CurrentCursorStatus = 2;
				if (vw_GetMouseLeftClick(true))
					Audio_PlaySound2D(7,1.0f);
			}
		}

		return false;
	}



	bool ON = false;
	bool CanClick = false;
	float IntTransp = Transp;


	// работаем с клавиатурой
	if ((Transp >= 0.99f)  && !isDialogBoxDrawing() && DrawGameCursor) CurrentActiveMenuElement++;
	bool InFocusByKeyboard = false;
	if (CurrentKeyboardSelectMenuElement > 0) {
		if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement) {
			InFocusByKeyboard = true;
		}
	}



	DstRect(X+2,Y+1,X+256,Y+63);
	if  ((vw_MouseOverRect(DstRect) || InFocusByKeyboard) && !isDialogBoxDrawing() && DrawGameCursor) {
		// если тухнем или появляемся - не жать
		ON = true;
		if (Transp==1.0f) {
			CanClick = true;
			CurrentCursorStatus = 1;
		}

		if (*ButTransp == 1.0f) {
			Audio_PlaySound2D(1,1.0f);
			*Update = vw_GetTimeThread(0);
			*ButTransp = 0.98f;
		}

		(*ButTransp) -= 3.0f*(vw_GetTimeThread(0) - (*Update));
		if (*ButTransp < 0.60f) *ButTransp = 0.60f;
		*Update = vw_GetTimeThread(0);

		IntTransp = (*ButTransp) * Transp;
	} else {
		if (*ButTransp < 1.0f) {
			(*ButTransp) += 3.0f*(vw_GetTimeThread(0) - (*Update));
			if (*ButTransp > 1.0f) *ButTransp =1.0f;
			*Update = vw_GetTimeThread(0);
		}

		IntTransp = (*ButTransp) * Transp;
	}







	// размер задней тени
	SrcRect(2,2,512-2,96-2 );
	// рисуем тень
	DstRect(X-125+2,Y-16+2,X-125+512-2,Y-16+96-2);
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/button256_back.tga"), true, IntTransp);

	SrcRect(0,0,256,64 );
	// рисуем кнопку
	DstRect(X,Y,X+256,Y+64);
	if (!ON)
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/button256_out.tga"), true, Transp);
	else
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/button256_in.tga"), true, Transp);


	// получаем длину текста
	int Size = vw_TextWidth(Text);

	// если текст сильно большой - сжимаем буквы, чтобы не вылазило за пределы кнопки
	float WScale = 0;
	if (Size > 190) {
		Size = 190;
		WScale = -190;
	}

	// находим смещение текста
	int SizeI = DstRect.left + (SrcRect.right-SrcRect.left-Size)/2;
	// рисуем текст
	if (!ON)
		vw_DrawText(SizeI, Y+21, WScale, 0, 1.0f, eRGBCOLOR::white, (0.7f*Transp)/2.0f, Text);
	else
		vw_DrawText(SizeI, Y+21, WScale, 0, 1.0f, eRGBCOLOR::white, Transp, Text);

	if (CanClick)
		if (vw_GetMouseLeftClick(true) || (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN)))) {
			Audio_PlaySound2D(2,1.0f);
			if (InFocusByKeyboard) {
				vw_SetKeyStatus(SDLK_KP_ENTER, false);
				vw_SetKeyStatus(SDLK_RETURN, false);
			}
			return true;
		}

	return false;
}





//------------------------------------------------------------------------------------
// прорисовка кнопки - 200
//------------------------------------------------------------------------------------
bool DrawButton200_2(int X, int Y, const char *Text, float Transp, bool Off)
{
	sRECT SrcRect, DstRect, MouseRect;
	SrcRect(2,2,230-2,64-2);
	DstRect(X-14+2,Y-14+2,X+230-14-2,Y+64-14-2);
	MouseRect(X,Y,X+204,Y+35);

	// получаем длину текста
	int Size = vw_TextWidth(Text);

	// если текст сильно большой - сжимаем буквы, чтобы не вылазило за пределы кнопки
	float WScale = 0;
	if (Size > 176) {
		Size = 176;
		WScale = -176;
	}
	int SizeI = DstRect.left + (SrcRect.right-SrcRect.left-Size)/2;


	if (Off || DragWeapon) {
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/button_dialog200_off.tga"), true, Transp);

		vw_DrawText(SizeI, Y+6, WScale, 0, 1.0f, eRGBCOLOR::white, (0.7f*Transp)/2.0f, Text);

		DstRect(X,Y,X+204,Y+35);
		if  (vw_MouseOverRect(DstRect) && !isDialogBoxDrawing()) {
			if (Transp==1.0f && !DragWeapon) {
				CurrentCursorStatus = 2;
				if (vw_GetMouseLeftClick(true))
					Audio_PlaySound2D(7,1.0f);
			}
		}

		return false;
	}


	bool ON = false;
	bool CanClick = false;


	// работаем с клавиатурой
	if ((Transp >= 0.99f)  && !isDialogBoxDrawing() && DrawGameCursor) CurrentActiveMenuElement++;
	bool InFocusByKeyboard = false;
	if (CurrentKeyboardSelectMenuElement > 0) {
		if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement) {
			InFocusByKeyboard = true;
		}
	}


	if  ((vw_MouseOverRect(MouseRect) || InFocusByKeyboard)  && !isDialogBoxDrawing() && DrawGameCursor) {
		// если тухнем или появляемся - не жать
		ON = true;
		if (Transp==1.0f) {
			CanClick = true;
			CurrentCursorStatus = 1;
		}

		if (NeedPlayOnButtonSoundX != X || NeedPlayOnButtonSoundY != Y) {
			Audio_PlaySound2D(15,1.0f);
			NeedPlayOnButtonSoundX = X;
			NeedPlayOnButtonSoundY = Y;
		}
	} else {
		if (NeedPlayOnButtonSoundX == X && NeedPlayOnButtonSoundY == Y) {
			NeedPlayOnButtonSoundX = 0;
			NeedPlayOnButtonSoundY = 0;
		}
	}


	if (!ON)
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/button_dialog200_out.tga"), true, Transp);
	else
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/button_dialog200_in.tga"), true, Transp);

	// рисуем текст
	if (!ON)
		vw_DrawText(SizeI, Y+6, WScale, 0, 1.0f, eRGBCOLOR::white, (0.7f*Transp)/2.0f, Text);
	else
		vw_DrawText(SizeI, Y+6, WScale, 0, 1.0f, eRGBCOLOR::white, Transp, Text);

	if (CanClick)
		if (vw_GetMouseLeftClick(true) || (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN)))) {
			Audio_PlaySound2D(2,1.0f);
			if (InFocusByKeyboard) {
				vw_SetKeyStatus(SDLK_KP_ENTER, false);
				vw_SetKeyStatus(SDLK_RETURN, false);
			}
			return true;
		}

	return false;
}


//------------------------------------------------------------------------------------
// прорисовка кнопки - 128
//------------------------------------------------------------------------------------
bool DrawButton128_2(int X, int Y, const char *Text, float Transp, bool Off, bool SoundClick)
{
	sRECT SrcRect, DstRect, MouseRect;
	SrcRect(2,2,158-2,64-2);
	DstRect(X-14+2,Y-14+2,X+158-14-2,Y+64-14-2);
	MouseRect(X,Y,X+132,Y+35);

	// получаем длину текста
	int Size = vw_TextWidth(Text);

	// если текст сильно большой - сжимаем буквы, чтобы не вылазило за пределы кнопки
	float WScale = 0;
	if (Size > 108) {
		Size = 108;
		WScale = -108;
	}

	// находим смещение текста
	int SizeI = DstRect.left + (SrcRect.right-SrcRect.left-Size)/2;


	if (Off || DragWeapon) {
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/button_dialog128_off.tga"), true, Transp);

		vw_DrawText(SizeI, Y+6, WScale, 0, 1.0f, eRGBCOLOR::white, (0.7f*Transp)/2.0f, Text);

		DstRect(X,Y,X+132,Y+35);
		if  (vw_MouseOverRect(DstRect) && !isDialogBoxDrawing()) {
			if (Transp==1.0f && !DragWeapon) {
				CurrentCursorStatus = 2;
				if (vw_GetMouseLeftClick(true))
					Audio_PlaySound2D(7,1.0f);
			}
		}

		return false;
	}



	bool ON = false;
	bool CanClick = false;


	// работаем с клавиатурой
	if ((Transp >= 0.99f)  && !isDialogBoxDrawing() && DrawGameCursor) CurrentActiveMenuElement++;
	bool InFocusByKeyboard = false;
	if (CurrentKeyboardSelectMenuElement > 0) {
		if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement) {
			InFocusByKeyboard = true;
		}
	}


	if  ((vw_MouseOverRect(MouseRect) || InFocusByKeyboard)  && !isDialogBoxDrawing() && DrawGameCursor) {
		// если тухнем или появляемся - не жать
		ON = true;
		if (Transp==1.0f) {
			CanClick = true;
			CurrentCursorStatus = 1;
		}

		if (NeedPlayOnButtonSoundX != X || NeedPlayOnButtonSoundY != Y) {
			Audio_PlaySound2D(15,1.0f);
			NeedPlayOnButtonSoundX = X;
			NeedPlayOnButtonSoundY = Y;
		}
	} else {
		if (NeedPlayOnButtonSoundX == X && NeedPlayOnButtonSoundY == Y) {
			NeedPlayOnButtonSoundX = 0;
			NeedPlayOnButtonSoundY = 0;
		}
	}


	if (!ON)
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/button_dialog128_out.tga"), true, Transp);
	else
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/button_dialog128_in.tga"), true, Transp);

	// рисуем текст
	if (!ON)
		vw_DrawText(SizeI, Y+6, WScale, 0, 1.0f, eRGBCOLOR::white, (0.7f*Transp)/2.0f, Text);
	else
		vw_DrawText(SizeI, Y+6, WScale, 0, 1.0f, eRGBCOLOR::white, Transp, Text);

	if (CanClick)
		if (vw_GetMouseLeftClick(true) || (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN)))) {
			if (SoundClick) Audio_PlaySound2D(2,1.0f);
			if (InFocusByKeyboard) {
				vw_SetKeyStatus(SDLK_KP_ENTER, false);
				vw_SetKeyStatus(SDLK_RETURN, false);
			}
			return true;
		}

	return false;
}










//------------------------------------------------------------------------------------
// прорисовка чекбокса
//------------------------------------------------------------------------------------
void DrawCheckBox(int X, int Y, bool *CheckBoxStatus, const char *Text, float Transp)
{
	sRECT SrcRect, DstRect;

	// получаем длину текста
	int Size = vw_TextWidth(Text);

	bool ON = false;
	bool CanClick = false;


	// работаем с клавиатурой
	if ((Transp >= 0.99f)  && !isDialogBoxDrawing() && DrawGameCursor) CurrentActiveMenuElement++;
	bool InFocusByKeyboard = false;
	if (CurrentKeyboardSelectMenuElement > 0) {
		if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement) {
			InFocusByKeyboard = true;
		}
	}


	// 20 - расстояние между текстом
	DstRect(X+4,Y+4,X+40+20+Size,Y+40-4);
	if  ((vw_MouseOverRect(DstRect) || InFocusByKeyboard)  && !isDialogBoxDrawing() && DrawGameCursor) {
		// если тухнем или появляемся - не жать
		ON = true;
		if (Transp==1.0f) {
			CanClick = true;
			CurrentCursorStatus = 1;
		}
	}



	// рисуем
	SrcRect(0,0,40,38);
	DstRect(X,Y,X+40,Y+38);
	if (!ON || DragWeapon)
		vw_DrawText(X+40+20, Y+8, 0, 0, 1.0f, eRGBCOLOR::white, Transp, Text);
	else
		vw_DrawText(X+40+20, Y+8, 0, 0, 1.0f, eRGBCOLOR::orange, Transp, Text);

	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/checkbox_main.tga"), true, Transp);
	if (*CheckBoxStatus)
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/checkbox_in.tga"), true, Transp);


	if (CanClick && !DragWeapon)
		if (vw_GetMouseLeftClick(true) || (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN)))) {
			*CheckBoxStatus = !(*CheckBoxStatus);
			Audio_PlaySound2D(2,1.0f);
			if (InFocusByKeyboard) {
				vw_SetKeyStatus(SDLK_KP_ENTER, false);
				vw_SetKeyStatus(SDLK_RETURN, false);
			}
		}
}





//------------------------------------------------------------------------------------
// прорисовка кнопки вверх для списков
//------------------------------------------------------------------------------------
bool DrawListUpButton(int X, int Y, float Transp, bool Off)
{
	sRECT SrcRect, DstRect, MouseRect;
	SrcRect(0,0,32,32);
	DstRect(X,Y,X+32,Y+32);
	MouseRect(X,Y,X+32,Y+32);


	if (Off || DragWeapon) {
		DstRect(X+2,Y+2,X+32-2,Y+32-2);
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/arrow_list_up.tga"), true, 0.3f*Transp);

		if  (vw_MouseOverRect(MouseRect) && !isDialogBoxDrawing()) {
			if (Transp==1.0f && !DragWeapon) {
				CurrentCursorStatus = 2;
				if (vw_GetMouseLeftClick(true))
					Audio_PlaySound2D(7,1.0f);
			}
		}

		return false;
	}



	bool ON = false;
	bool CanClick = false;


	// работаем с клавиатурой
	if ((Transp >= 0.99f)  && !isDialogBoxDrawing() && DrawGameCursor) CurrentActiveMenuElement++;
	bool InFocusByKeyboard = false;
	if (CurrentKeyboardSelectMenuElement > 0) {
		if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement) {
			InFocusByKeyboard = true;
		}
	}


	if  ((vw_MouseOverRect(MouseRect) || InFocusByKeyboard)  && !isDialogBoxDrawing() && DrawGameCursor) {
		// если тухнем или появляемся - не жать
		ON = true;
		if (Transp==1.0f) {
			CanClick = true;
			CurrentCursorStatus = 1;
		}

		if (NeedPlayOnButtonSoundX != X || NeedPlayOnButtonSoundY != Y) {
			Audio_PlaySound2D(15,1.0f);
			NeedPlayOnButtonSoundX = X;
			NeedPlayOnButtonSoundY = Y;
		}
	} else {
		if (NeedPlayOnButtonSoundX == X && NeedPlayOnButtonSoundY == Y) {
			NeedPlayOnButtonSoundX = 0;
			NeedPlayOnButtonSoundY = 0;
		}
	}


	if (!ON) {
		DstRect(X+2,Y+2,X+32-2,Y+32-2);
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/arrow_list_up.tga"), true, 0.3f*Transp);
	} else
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/arrow_list_up.tga"), true, Transp);


	if (CanClick)
		if (vw_GetMouseLeftClick(true) || (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN)))) {
			Audio_PlaySound2D(2,1.0f);
			if (InFocusByKeyboard) {
				vw_SetKeyStatus(SDLK_KP_ENTER, false);
				vw_SetKeyStatus(SDLK_RETURN, false);
			}
			return true;
		}

	return false;
}



//------------------------------------------------------------------------------------
// прорисовка кнопки вниз для списков
//------------------------------------------------------------------------------------
bool DrawListDownButton(int X, int Y, float Transp, bool Off)
{
	sRECT SrcRect, DstRect, MouseRect;
	SrcRect(0,0,32,32);
	DstRect(X,Y,X+32,Y+32);
	MouseRect(X,Y,X+32,Y+32);


	if (Off || DragWeapon) {
		DstRect(X+2,Y+2,X+32-2,Y+32-2);
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/arrow_list_down.tga"), true, 0.3f*Transp);

		if  (vw_MouseOverRect(MouseRect) && !isDialogBoxDrawing()) {
			if (Transp==1.0f && !DragWeapon) {
				CurrentCursorStatus = 2;
				if (vw_GetMouseLeftClick(true))
					Audio_PlaySound2D(7,1.0f);
			}
		}

		return false;
	}



	bool ON = false;
	bool CanClick = false;


	// работаем с клавиатурой
	if ((Transp >= 0.99f)  && !isDialogBoxDrawing() && DrawGameCursor) CurrentActiveMenuElement++;
	bool InFocusByKeyboard = false;
	if (CurrentKeyboardSelectMenuElement > 0) {
		if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement) {
			InFocusByKeyboard = true;
		}
	}


	if  ((vw_MouseOverRect(MouseRect) || InFocusByKeyboard)  && !isDialogBoxDrawing() && DrawGameCursor) {
		// если тухнем или появляемся - не жать
		ON = true;
		if (Transp==1.0f) {
			CanClick = true;
			CurrentCursorStatus = 1;
		}

		if (NeedPlayOnButtonSoundX != X || NeedPlayOnButtonSoundY != Y) {
			Audio_PlaySound2D(15,1.0f);
			NeedPlayOnButtonSoundX = X;
			NeedPlayOnButtonSoundY = Y;
		}
	} else {
		if (NeedPlayOnButtonSoundX == X && NeedPlayOnButtonSoundY == Y) {
			NeedPlayOnButtonSoundX = 0;
			NeedPlayOnButtonSoundY = 0;
		}
	}


	if (!ON) {
		DstRect(X+2,Y+2,X+32-2,Y+32-2);
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/arrow_list_down.tga"), true, 0.3f*Transp);
	} else
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/arrow_list_down.tga"), true, Transp);


	if (CanClick)
		if (vw_GetMouseLeftClick(true) || (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN)))) {
			Audio_PlaySound2D(2,1.0f);
			if (InFocusByKeyboard) {
				vw_SetKeyStatus(SDLK_KP_ENTER, false);
				vw_SetKeyStatus(SDLK_RETURN, false);
			}
			return true;
		}

	return false;
}
