/************************************************************************************

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


	Web Site: http://www.viewizard.com/
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


#include "../Game.h"


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
	RECT SrcRest, DstRest;


	// все проверки
	int MouseX, MouseY;
	vw_GetMousePos(&MouseX, &MouseY);

	bool ON = false;
	bool CanClick = false;
	float IntTransp = Transp;



	// работаем с клавиатурой
	if ((Transp >= 0.99f) && !isDialogBoxDrawing() && DrawGameCursor) CurrentActiveMenuElement++;
	bool InFocusByKeyboard = false;
	if (CurrentKeyboardSelectMenuElement > 0)
	{
		if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement)
		{
			InFocusByKeyboard = true;
		}
	}



	SetRect(&DstRest,X+2,Y+1,X+384,Y+63);
	if  ((((DstRest.right  >= MouseX) &&
		(DstRest.left<= MouseX) &&
		(DstRest.bottom >= MouseY) &&
		(DstRest.top<= MouseY)) || InFocusByKeyboard) && !isDialogBoxDrawing() && DrawGameCursor)
	{
		// если тухнем или появляемся - не жать
		ON = true;
		if (Transp==1.0f)
		{
			CanClick = true;
			CurrentCursorStatus = 1;
		}


		if (*ButTransp == 1.0f)
		{
			Audio_PlaySound2D(1,1.0f);
			*Update = vw_GetTime();
			*ButTransp = 0.99f;
		}

		(*ButTransp) -= 3.0f*(vw_GetTime() - (*Update));
		if (*ButTransp < 0.60f) *ButTransp = 0.60f;
		*Update = vw_GetTime();

		IntTransp = (*ButTransp) * Transp;
	}
	else
	{
		if (*ButTransp < 1.0f)
		{
			(*ButTransp) += 3.0f*(vw_GetTime() - (*Update));
			if (*ButTransp > 1.0f) *ButTransp =1.0f;
			*Update = vw_GetTime();
		}

		IntTransp = (*ButTransp) * Transp;
	}



	// размер задней тени
	SetRect(&SrcRest,2,2,512-2,96-2 );
	// рисуем тень
	SetRect(&DstRest,X-64+2,Y-17+2,X-64+512-2,Y-17+96-2);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/button384_back.tga"),
		true, IntTransp, 0.0f, RI_UL_CORNER, 1.0f, 1.0f, 1.0f);

	SetRect(&SrcRest,0,0,384,64 );
	// рисуем кнопку
	SetRect(&DstRest,X,Y,X+384,Y+64);
	if (!ON)
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/button384_out.tga"), true, Transp);
	else
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/button384_in.tga"), true, Transp);



	// получаем длину текста
	int Size = vw_FontSize(Text);

	// если текст сильно большой - сжимаем буквы, чтобы не вылазило за пределы кнопки
	float WScale = 0;
	if (Size > 310)
	{
		Size = 310;
		WScale = -310;
	}

	// находим смещение текста
	int SizeI = X + (SrcRest.right-SrcRest.left-Size)/2;

	// рисуем текст
	if (!ON)
		vw_DrawFont(SizeI, Y+21, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, (0.7f*Transp)/2.0f, Text);
	else
		vw_DrawFont(SizeI, Y+21, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, Transp, Text);



	if (CanClick)
		if (vw_GetWindowLBMouse(true) || (InFocusByKeyboard && (vw_GetKeys(SDLK_KP_ENTER) || vw_GetKeys(SDLK_RETURN))))
		{
			Audio_PlaySound2D(2,1.0f);
			if (InFocusByKeyboard)
			{
				vw_SetKeys(SDLK_KP_ENTER, false);
				vw_SetKeys(SDLK_RETURN, false);
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
	RECT SrcRest, DstRest;
	// все проверки
	int MouseX, MouseY;
	vw_GetMousePos(&MouseX, &MouseY);


	if (Off || DragWeapon)
	{

		SetRect(&SrcRest,2,2,512-2,96-2 );
		SetRect(&DstRest,X-125+2,Y-16+2,X-125+512-2,Y-16+96-2);
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/button256_back.tga"),
		true, Transp, 0.0f, RI_UL_CORNER, 1.0f, 1.0f, 1.0f);

		SetRect(&SrcRest,0,0,256,64 );
		SetRect(&DstRest,X,Y,X+256,Y+64);
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/button256_off.tga"), true, Transp);


		int Size = vw_FontSize(Text);
		int SizeI = DstRest.left + (SrcRest.right-SrcRest.left-Size)/2;
		vw_DrawFont(SizeI, Y+21, 0, 0, 1.0f, 1.0f,1.0f,1.0f, (0.7f*Transp)/2.0f, Text);

		SetRect(&DstRest,X+2,Y+1,X+256,Y+63);
		if  (((DstRest.right  >= MouseX) &&
			(DstRest.left<= MouseX) &&
			(DstRest.bottom >= MouseY) &&
			(DstRest.top<= MouseY)) && !isDialogBoxDrawing())
		{
			if (Transp==1.0f && !DragWeapon)
			{
				CurrentCursorStatus = 2;
				if (vw_GetWindowLBMouse(true))
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
	if (CurrentKeyboardSelectMenuElement > 0)
	{
		if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement)
		{
			InFocusByKeyboard = true;
		}
	}



	SetRect(&DstRest,X+2,Y+1,X+256,Y+63);
	if  ((((DstRest.right  >= MouseX) &&
		(DstRest.left<= MouseX) &&
		(DstRest.bottom >= MouseY) &&
		(DstRest.top<= MouseY)) || InFocusByKeyboard) && !isDialogBoxDrawing() && DrawGameCursor)
	{
		// если тухнем или появляемся - не жать
		ON = true;
		if (Transp==1.0f)
		{
			CanClick = true;
			CurrentCursorStatus = 1;
		}

		if (*ButTransp == 1.0f)
		{
			Audio_PlaySound2D(1,1.0f);
			*Update = vw_GetTime();
			*ButTransp = 0.98f;
		}

		(*ButTransp) -= 3.0f*(vw_GetTime() - (*Update));
		if (*ButTransp < 0.60f) *ButTransp = 0.60f;
		*Update = vw_GetTime();

		IntTransp = (*ButTransp) * Transp;
	}
	else
	{
		if (*ButTransp < 1.0f)
		{
			(*ButTransp) += 3.0f*(vw_GetTime() - (*Update));
			if (*ButTransp > 1.0f) *ButTransp =1.0f;
			*Update = vw_GetTime();
		}

		IntTransp = (*ButTransp) * Transp;
	}







	// размер задней тени
	SetRect(&SrcRest,2,2,512-2,96-2 );
	// рисуем тень
	SetRect(&DstRest,X-125+2,Y-16+2,X-125+512-2,Y-16+96-2);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/button256_back.tga"),
		true, IntTransp, 0.0f, RI_UL_CORNER, 1.0f, 1.0f, 1.0f);

	SetRect(&SrcRest,0,0,256,64 );
	// рисуем кнопку
	SetRect(&DstRest,X,Y,X+256,Y+64);
	if (!ON)
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/button256_out.tga"), true, Transp);
	else
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/button256_in.tga"), true, Transp);


	// получаем длину текста
	int Size = vw_FontSize(Text);

	// если текст сильно большой - сжимаем буквы, чтобы не вылазило за пределы кнопки
	float WScale = 0;
	if (Size > 190)
	{
		Size = 190;
		WScale = -190;
	}

	// находим смещение текста
	int SizeI = DstRest.left + (SrcRest.right-SrcRest.left-Size)/2;
	// рисуем текст
	if (!ON)
		vw_DrawFont(SizeI, Y+21, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, (0.7f*Transp)/2.0f, Text);
	else
		vw_DrawFont(SizeI, Y+21, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, Transp, Text);

	if (CanClick)
		if (vw_GetWindowLBMouse(true) || (InFocusByKeyboard && (vw_GetKeys(SDLK_KP_ENTER) || vw_GetKeys(SDLK_RETURN))))
		{
			Audio_PlaySound2D(2,1.0f);
			if (InFocusByKeyboard)
			{
				vw_SetKeys(SDLK_KP_ENTER, false);
				vw_SetKeys(SDLK_RETURN, false);
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
	RECT SrcRest, DstRest, MouseRest;
	SetRect(&SrcRest,2,2,230-2,64-2);
	SetRect(&DstRest,X-14+2,Y-14+2,X+230-14-2,Y+64-14-2);
	SetRect(&MouseRest,X,Y,X+204,Y+35);

	// получаем длину текста
	int Size = vw_FontSize(Text);

	// если текст сильно большой - сжимаем буквы, чтобы не вылазило за пределы кнопки
	float WScale = 0;
	if (Size > 176)
	{
		Size = 176;
		WScale = -176;
	}
	int SizeI = DstRest.left + (SrcRest.right-SrcRest.left-Size)/2;

	// все проверки
	int MouseX, MouseY;
	vw_GetMousePos(&MouseX, &MouseY);


	if (Off || DragWeapon)
	{
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/button_dialog200_off.tga"), true, Transp);

		vw_DrawFont(SizeI, Y+6, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, (0.7f*Transp)/2.0f, Text);

		SetRect(&DstRest,X,Y,X+204,Y+35);
		if  (((DstRest.right  >= MouseX) &&
			(DstRest.left<= MouseX) &&
			(DstRest.bottom >= MouseY) &&
			(DstRest.top<= MouseY)) && !isDialogBoxDrawing())
		{
			if (Transp==1.0f && !DragWeapon)
			{
				CurrentCursorStatus = 2;
				if (vw_GetWindowLBMouse(true))
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
	if (CurrentKeyboardSelectMenuElement > 0)
	{
		if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement)
		{
			InFocusByKeyboard = true;
		}
	}


	if  ((((MouseRest.right  >= MouseX) &&
		(MouseRest.left<= MouseX) &&
		(MouseRest.bottom >= MouseY) &&
		(MouseRest.top<= MouseY)) || InFocusByKeyboard)  && !isDialogBoxDrawing() && DrawGameCursor)
	{
		// если тухнем или появляемся - не жать
		ON = true;
		if (Transp==1.0f)
		{
			CanClick = true;
			CurrentCursorStatus = 1;
		}

		if (NeedPlayOnButtonSoundX != X || NeedPlayOnButtonSoundY != Y)
		{
			Audio_PlaySound2D(15,1.0f);
			NeedPlayOnButtonSoundX = X;
			NeedPlayOnButtonSoundY = Y;
		}
	}
	else
	{
		if (NeedPlayOnButtonSoundX == X && NeedPlayOnButtonSoundY == Y)
		{
			NeedPlayOnButtonSoundX = 0;
			NeedPlayOnButtonSoundY = 0;
		}
	}


	if (!ON)
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/button_dialog200_out.tga"), true, Transp);
	else
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/button_dialog200_in.tga"), true, Transp);

	// рисуем текст
	if (!ON)
		vw_DrawFont(SizeI, Y+6, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, (0.7f*Transp)/2.0f, Text);
	else
		vw_DrawFont(SizeI, Y+6, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, Transp, Text);

	if (CanClick)
		if (vw_GetWindowLBMouse(true) || (InFocusByKeyboard && (vw_GetKeys(SDLK_KP_ENTER) || vw_GetKeys(SDLK_RETURN))))
		{
			Audio_PlaySound2D(2,1.0f);
			if (InFocusByKeyboard)
			{
				vw_SetKeys(SDLK_KP_ENTER, false);
				vw_SetKeys(SDLK_RETURN, false);
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
	RECT SrcRest, DstRest, MouseRest;
	SetRect(&SrcRest,2,2,158-2,64-2);
	SetRect(&DstRest,X-14+2,Y-14+2,X+158-14-2,Y+64-14-2);
	SetRect(&MouseRest,X,Y,X+132,Y+35);

	// все проверки
	int MouseX, MouseY;
	vw_GetMousePos(&MouseX, &MouseY);

	// получаем длину текста
	int Size = vw_FontSize(Text);

	// если текст сильно большой - сжимаем буквы, чтобы не вылазило за пределы кнопки
	float WScale = 0;
	if (Size > 108)
	{
		Size = 108;
		WScale = -108;
	}

	// находим смещение текста
	int SizeI = DstRest.left + (SrcRest.right-SrcRest.left-Size)/2;


	if (Off || DragWeapon)
	{
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/button_dialog128_off.tga"), true, Transp);

		vw_DrawFont(SizeI, Y+6, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, (0.7f*Transp)/2.0f, Text);

		SetRect(&DstRest,X,Y,X+132,Y+35);
		if  (((DstRest.right  >= MouseX) &&
			(DstRest.left<= MouseX) &&
			(DstRest.bottom >= MouseY) &&
			(DstRest.top<= MouseY)) && !isDialogBoxDrawing())
		{
			if (Transp==1.0f && !DragWeapon)
			{
				CurrentCursorStatus = 2;
				if (vw_GetWindowLBMouse(true))
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
	if (CurrentKeyboardSelectMenuElement > 0)
	{
		if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement)
		{
			InFocusByKeyboard = true;
		}
	}


	if  ((((MouseRest.right  >= MouseX) &&
		(MouseRest.left<= MouseX) &&
		(MouseRest.bottom >= MouseY) &&
		(MouseRest.top<= MouseY)) || InFocusByKeyboard)  && !isDialogBoxDrawing() && DrawGameCursor)
	{
		// если тухнем или появляемся - не жать
		ON = true;
		if (Transp==1.0f)
		{
			CanClick = true;
			CurrentCursorStatus = 1;
		}

		if (NeedPlayOnButtonSoundX != X || NeedPlayOnButtonSoundY != Y)
		{
			Audio_PlaySound2D(15,1.0f);
			NeedPlayOnButtonSoundX = X;
			NeedPlayOnButtonSoundY = Y;
		}
	}
	else
	{
		if (NeedPlayOnButtonSoundX == X && NeedPlayOnButtonSoundY == Y)
		{
			NeedPlayOnButtonSoundX = 0;
			NeedPlayOnButtonSoundY = 0;
		}
	}


	if (!ON)
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/button_dialog128_out.tga"), true, Transp);
	else
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/button_dialog128_in.tga"), true, Transp);

	// рисуем текст
	if (!ON)
		vw_DrawFont(SizeI, Y+6, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, (0.7f*Transp)/2.0f, Text);
	else
		vw_DrawFont(SizeI, Y+6, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, Transp, Text);

	if (CanClick)
		if (vw_GetWindowLBMouse(true) || (InFocusByKeyboard && (vw_GetKeys(SDLK_KP_ENTER) || vw_GetKeys(SDLK_RETURN))))
		{
			if (SoundClick) Audio_PlaySound2D(2,1.0f);
			if (InFocusByKeyboard)
			{
				vw_SetKeys(SDLK_KP_ENTER, false);
				vw_SetKeys(SDLK_RETURN, false);
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
	RECT SrcRest, DstRest;
	// все проверки
	int MouseX, MouseY;
	vw_GetMousePos(&MouseX, &MouseY);


	// получаем длину текста
	int Size = vw_FontSize(Text);

	bool ON = false;
	bool CanClick = false;


	// работаем с клавиатурой
	if ((Transp >= 0.99f)  && !isDialogBoxDrawing() && DrawGameCursor) CurrentActiveMenuElement++;
	bool InFocusByKeyboard = false;
	if (CurrentKeyboardSelectMenuElement > 0)
	{
		if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement)
		{
			InFocusByKeyboard = true;
		}
	}


	// 20 - расстояние между текстом
	SetRect(&DstRest,X+4,Y+4,X+40+20+Size,Y+40-4);
	if  ((((DstRest.right  >= MouseX) &&
		(DstRest.left<= MouseX) &&
		(DstRest.bottom >= MouseY) &&
		(DstRest.top<= MouseY)) || InFocusByKeyboard)  && !isDialogBoxDrawing() && DrawGameCursor)
	{
		// если тухнем или появляемся - не жать
		ON = true;
		if (Transp==1.0f)
		{
			CanClick = true;
			CurrentCursorStatus = 1;
		}
	}



	// рисуем
	SetRect(&SrcRest,0,0,40,38);
	SetRect(&DstRest,X,Y,X+40,Y+38);
	if (!ON || DragWeapon)
		vw_DrawFont(X+40+20, Y+8, 0, 0, 1.0f, 1.0f,1.0f,1.0f, Transp, Text);
	else
		vw_DrawFont(X+40+20, Y+8, 0, 0, 1.0f, 1.0f,0.5f,0.0f, Transp, Text);

	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/checkbox_main.tga"), true, Transp);
	if (*CheckBoxStatus)
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/checkbox_in.tga"), true, Transp);


	if (CanClick && !DragWeapon)
		if (vw_GetWindowLBMouse(true) || (InFocusByKeyboard && (vw_GetKeys(SDLK_KP_ENTER) || vw_GetKeys(SDLK_RETURN))))
		{
			*CheckBoxStatus = !(*CheckBoxStatus);
			Audio_PlaySound2D(2,1.0f);
			if (InFocusByKeyboard)
			{
				vw_SetKeys(SDLK_KP_ENTER, false);
				vw_SetKeys(SDLK_RETURN, false);
			}
		}
}





//------------------------------------------------------------------------------------
// прорисовка кнопки вверх для списков
//------------------------------------------------------------------------------------
bool DrawListUpButton(int X, int Y, float Transp, bool Off)
{
	RECT SrcRest, DstRest, MouseRest;
	SetRect(&SrcRest,0,0,32,32);
	SetRect(&DstRest,X,Y,X+32,Y+32);
	SetRect(&MouseRest,X,Y,X+32,Y+32);

	// все проверки
	int MouseX, MouseY;
	vw_GetMousePos(&MouseX, &MouseY);


	if (Off || DragWeapon)
	{
		SetRect(&DstRest,X+2,Y+2,X+32-2,Y+32-2);
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/arrow_list_up.tga"), true, 0.3f*Transp);

		if  (((MouseRest.right  >= MouseX) &&
			(MouseRest.left<= MouseX) &&
			(MouseRest.bottom >= MouseY) &&
			(MouseRest.top<= MouseY)) && !isDialogBoxDrawing())
		{
			if (Transp==1.0f && !DragWeapon)
			{
				CurrentCursorStatus = 2;
				if (vw_GetWindowLBMouse(true))
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
	if (CurrentKeyboardSelectMenuElement > 0)
	{
		if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement)
		{
			InFocusByKeyboard = true;
		}
	}


	if  ((((MouseRest.right  >= MouseX) &&
		(MouseRest.left<= MouseX) &&
		(MouseRest.bottom >= MouseY) &&
		(MouseRest.top<= MouseY)) || InFocusByKeyboard)  && !isDialogBoxDrawing() && DrawGameCursor)
	{
		// если тухнем или появляемся - не жать
		ON = true;
		if (Transp==1.0f)
		{
			CanClick = true;
			CurrentCursorStatus = 1;
		}

		if (NeedPlayOnButtonSoundX != X || NeedPlayOnButtonSoundY != Y)
		{
			Audio_PlaySound2D(15,1.0f);
			NeedPlayOnButtonSoundX = X;
			NeedPlayOnButtonSoundY = Y;
		}
	}
	else
	{
		if (NeedPlayOnButtonSoundX == X && NeedPlayOnButtonSoundY == Y)
		{
			NeedPlayOnButtonSoundX = 0;
			NeedPlayOnButtonSoundY = 0;
		}
	}


	if (!ON)
	{
		SetRect(&DstRest,X+2,Y+2,X+32-2,Y+32-2);
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/arrow_list_up.tga"), true, 0.3f*Transp);
	}
	else
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/arrow_list_up.tga"), true, Transp);


	if (CanClick)
		if (vw_GetWindowLBMouse(true) || (InFocusByKeyboard && (vw_GetKeys(SDLK_KP_ENTER) || vw_GetKeys(SDLK_RETURN))))
		{
			Audio_PlaySound2D(2,1.0f);
			if (InFocusByKeyboard)
			{
				vw_SetKeys(SDLK_KP_ENTER, false);
				vw_SetKeys(SDLK_RETURN, false);
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
	RECT SrcRest, DstRest, MouseRest;
	SetRect(&SrcRest,0,0,32,32);
	SetRect(&DstRest,X,Y,X+32,Y+32);
	SetRect(&MouseRest,X,Y,X+32,Y+32);

	// все проверки
	int MouseX, MouseY;
	vw_GetMousePos(&MouseX, &MouseY);


	if (Off || DragWeapon)
	{
		SetRect(&DstRest,X+2,Y+2,X+32-2,Y+32-2);
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/arrow_list_down.tga"), true, 0.3f*Transp);

		if  (((MouseRest.right  >= MouseX) &&
			(MouseRest.left<= MouseX) &&
			(MouseRest.bottom >= MouseY) &&
			(MouseRest.top<= MouseY)) && !isDialogBoxDrawing())
		{
			if (Transp==1.0f && !DragWeapon)
			{
				CurrentCursorStatus = 2;
				if (vw_GetWindowLBMouse(true))
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
	if (CurrentKeyboardSelectMenuElement > 0)
	{
		if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement)
		{
			InFocusByKeyboard = true;
		}
	}


	if  ((((MouseRest.right  >= MouseX) &&
		(MouseRest.left<= MouseX) &&
		(MouseRest.bottom >= MouseY) &&
		(MouseRest.top<= MouseY)) || InFocusByKeyboard)  && !isDialogBoxDrawing() && DrawGameCursor)
	{
		// если тухнем или появляемся - не жать
		ON = true;
		if (Transp==1.0f)
		{
			CanClick = true;
			CurrentCursorStatus = 1;
		}

		if (NeedPlayOnButtonSoundX != X || NeedPlayOnButtonSoundY != Y)
		{
			Audio_PlaySound2D(15,1.0f);
			NeedPlayOnButtonSoundX = X;
			NeedPlayOnButtonSoundY = Y;
		}
	}
	else
	{
		if (NeedPlayOnButtonSoundX == X && NeedPlayOnButtonSoundY == Y)
		{
			NeedPlayOnButtonSoundX = 0;
			NeedPlayOnButtonSoundY = 0;
		}
	}


	if (!ON)
	{
		SetRect(&DstRest,X+2,Y+2,X+32-2,Y+32-2);
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/arrow_list_down.tga"), true, 0.3f*Transp);
	}
	else
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/arrow_list_down.tga"), true, Transp);


	if (CanClick)
		if (vw_GetWindowLBMouse(true) || (InFocusByKeyboard && (vw_GetKeys(SDLK_KP_ENTER) || vw_GetKeys(SDLK_RETURN))))
		{
			Audio_PlaySound2D(2,1.0f);
			if (InFocusByKeyboard)
			{
				vw_SetKeys(SDLK_KP_ENTER, false);
				vw_SetKeys(SDLK_RETURN, false);
			}
			return true;
		}

	return false;
}

