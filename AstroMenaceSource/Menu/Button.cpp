/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Button.cpp

	Copyright (c) 2006-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.3

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


#include "../Game.h"


extern bool DragWeapon;



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

	SetRect(&DstRest,X+2,Y+1,X+384,Y+63);
	if  (((DstRest.right  >= MouseX)&
		(DstRest.left<= MouseX)&
		(DstRest.bottom >= MouseY)&
		(DstRest.top<= MouseY)) && !isDialogBoxDrawing() && DrawGameCursor)
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
			Audio_PlayMenuSound(1,1.0f);
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
		if (vw_GetWindowLBMouse(true))
		{
			Audio_PlayMenuSound(2,1.0f);
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
		if  (((DstRest.right  >= MouseX)&
			(DstRest.left<= MouseX)&
			(DstRest.bottom >= MouseY)&
			(DstRest.top<= MouseY)) && !isDialogBoxDrawing())
		{
			if (Transp==1.0f && !DragWeapon)
			{
				CurrentCursorStatus = 2;
				if (vw_GetWindowLBMouse(true))
					Audio_PlayMenuSound(7,1.0f);
			}
		}

		return false;
	}



	bool ON = false;
	bool CanClick = false;
	float IntTransp = Transp;

	SetRect(&DstRest,X+2,Y+1,X+256,Y+63);
	if  (((DstRest.right  >= MouseX)&
		(DstRest.left<= MouseX)&
		(DstRest.bottom >= MouseY)&
		(DstRest.top<= MouseY)) && !isDialogBoxDrawing() && DrawGameCursor)
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
			Audio_PlayMenuSound(1,1.0f);
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
		if (vw_GetWindowLBMouse(true))
		{
			Audio_PlayMenuSound(2,1.0f);
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
		if  (((DstRest.right  >= MouseX)&
			(DstRest.left<= MouseX)&
			(DstRest.bottom >= MouseY)&
			(DstRest.top<= MouseY))  && !isDialogBoxDrawing())
		{
			if (Transp==1.0f && !DragWeapon)
			{
				CurrentCursorStatus = 2;
				if (vw_GetWindowLBMouse(true))
					Audio_PlayMenuSound(7,1.0f);
			}
		}

		return false;
	}


	bool ON = false;
	bool CanClick = false;


	if  (((MouseRest.right  >= MouseX)&
		(MouseRest.left<= MouseX)&
		(MouseRest.bottom >= MouseY)&
		(MouseRest.top<= MouseY))  && !isDialogBoxDrawing() && DrawGameCursor)
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
			Audio_PlayMenuSound(25,1.0f);
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
		if (vw_GetWindowLBMouse(true))
		{
			Audio_PlayMenuSound(2,1.0f);
			return true;
		}

	return false;
}


//------------------------------------------------------------------------------------
// прорисовка кнопки - 128
//------------------------------------------------------------------------------------
bool DrawButton128_2(int X, int Y, const char *Text, float Transp, bool Off)
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
		if  (((DstRest.right  >= MouseX)&
			(DstRest.left<= MouseX)&
			(DstRest.bottom >= MouseY)&
			(DstRest.top<= MouseY))  && !isDialogBoxDrawing())
		{
			if (Transp==1.0f && !DragWeapon)
			{
				CurrentCursorStatus = 2;
				if (vw_GetWindowLBMouse(true))
					Audio_PlayMenuSound(7,1.0f);
			}
		}

		return false;
	}



	bool ON = false;
	bool CanClick = false;

	if  (((MouseRest.right  >= MouseX)&
		(MouseRest.left<= MouseX)&
		(MouseRest.bottom >= MouseY)&
		(MouseRest.top<= MouseY))  && !isDialogBoxDrawing() && DrawGameCursor)
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
			Audio_PlayMenuSound(25,1.0f);
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
		if (vw_GetWindowLBMouse(true))
		{
			Audio_PlayMenuSound(2,1.0f);
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

	// 20 - расстояние между текстом
	SetRect(&DstRest,X+4,Y+4,X+40+20+Size,Y+40-4);
	if  (((DstRest.right  >= MouseX)&
		(DstRest.left<= MouseX)&
		(DstRest.bottom >= MouseY)&
		(DstRest.top<= MouseY))  && !isDialogBoxDrawing() && DrawGameCursor)
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
		if (vw_GetWindowLBMouse(true))
		{
			*CheckBoxStatus = !(*CheckBoxStatus);
			Audio_PlayMenuSound(2,1.0f);
		}
}
