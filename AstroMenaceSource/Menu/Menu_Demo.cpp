/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Menu_Demo.cpp

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.2

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


#include "../Defines.h"
#include "../Game.h"


// для индикации нужных надписей
float CurrentDemoShowTime = 4.0f;
float LastDemoShowTime = 0.0f;


void DemoMenu()
{
	// подсчет времени
	float TimeDelta = vw_GetTime() - LastDemoShowTime;
	CurrentDemoShowTime -= TimeDelta;
	if (CurrentDemoShowTime < 0.0f) CurrentDemoShowTime = 0.0f;
	LastDemoShowTime = vw_GetTime();




	RECT SrcRest, DstRest;

    int X2 = Setup.iAspectRatioWidth/2-112;



	SetRect(&SrcRest,0,0,2,2);
	SetRect(&DstRest,0,0,Setup.iAspectRatioWidth,768);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.5f*MenuContentTransp);




    // выводим рисунки 4-х уровней...
	SetRect(&SrcRest,0,0,256,192);
	SetRect(&DstRest,X2+90,180,X2+90+256,180+192);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/DEMO/image1.tga"), false, MenuContentTransp);
	SetRect(&SrcRest,0,0,2,2);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.2f*MenuContentTransp);


	SetRect(&SrcRest,0,0,256,192);
	SetRect(&DstRest,X2+290,230,X2+290+256,230+192);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/DEMO/image2.tga"), false, MenuContentTransp);
	SetRect(&SrcRest,0,0,2,2);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.2f*MenuContentTransp);

	SetRect(&SrcRest,0,0,256,192);
	SetRect(&DstRest,X2+50,360,X2+50+256,360+192);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/DEMO/image3.tga"), false, MenuContentTransp);
	SetRect(&SrcRest,0,0,2,2);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.2f*MenuContentTransp);

	SetRect(&SrcRest,0,0,256,192);
	SetRect(&DstRest,X2+250,410,X2+250+256,410+192);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/DEMO/image4.tga"), false, MenuContentTransp);
	SetRect(&SrcRest,0,0,2,2);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.2f*MenuContentTransp);






    // текст справа
    int X1 = Setup.iAspectRatioWidth/2-432;
    int Y1 = 170;
    int Step = 30;

	int	Size = FontSize(GetText("8_Line1"))/2;
	DrawFont(Setup.iAspectRatioWidth/2-Size, Y1, 0, 0, 4, MenuContentTransp, GetText("8_Line1"));

	// общие условия
    Y1 += Step*2;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("8_Line2"));
    Y1 += Step;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("8_Line3"));
    Y1 += Step;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("8_Line4"));


    // по настройкам корабля игрока
    Y1 += Step*2;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("8_Line5"));
    Y1 += Step;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("8_Line6"));
    Y1 += Step;
    DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("8_Line7"));



    Y1 += Step*2;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("8_Line8"));
    Y1 += Step;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("8_Line9"));
    Y1 += Step;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("8_Line10"));

    Y1 += Step*2;

	Size = FontSize(GetText("8_Line11"))/2;
	DrawFont(Setup.iAspectRatioWidth/2-Size, Y1, 0, 0, 2, MenuContentTransp, GetText("8_Line11"));



	int	X = Setup.iAspectRatioWidth/2-284;
	int Y = 165+100*5;

	char ButtonTitle[100];
	bool ONOFF = false;
	if (CurrentDemoShowTime > 1.0f)
	{
		ONOFF = true;
		sprintf(ButtonTitle,"%i", (int)CurrentDemoShowTime);
	}
	else
	{
		ONOFF = false;
		strcpy(ButtonTitle, GetText("1_QUIT"));
	}


	if (DrawButton256(X,Y, ButtonTitle, MenuContentTransp, &Button10Transp, &LastButton10UpdateTime, ONOFF))
	{
	    // выходим
		CanQuit = true;
		Quit = true;
	}
	X = Setup.iAspectRatioWidth/2+28;
	if (DrawButton256(X,Y, GetText("1_ORDER_NOW"), MenuContentTransp, &Button11Transp, &LastButton11UpdateTime))
	{
		// нужно свернуть игру, запустить броузер и выйти в основное меню
		SDL_WM_IconifyWindow();

#ifdef EN
		vw_OpenBrouser("http://www.viewizard.com/buynow.php?forceos=windows&forcegame=astromenace");
#endif // EN
#ifdef DE
		vw_OpenBrouser("http://www.viewizard.com/de/buynow.php?forceos=windows&forcegame=astromenace");
#endif // DE
#ifdef RU
		vw_OpenBrouser("http://www.viewizard.com/ru/buynow.php?forceos=windows&forcegame=astromenace");
#endif // RU
	}


}


