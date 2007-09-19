/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: ScriptFunction.cpp

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


#include "../Game.h"




//-----------------------------------------------------------------------------
// aimode
//-----------------------------------------------------------------------------
void SetAIMode(CObject3D *Object, TiXmlElement *Element)
{
	if (Element->Attribute("aimode"))
	{
		// собираем новый элемент
		CTimeSheet *TimeSheet;
		TimeSheet = new CTimeSheet;
		Object->AttachTimeSheet(TimeSheet);

		TimeSheet->Time = -1;
		TimeSheet->AI_Mode = atoi(Element->Attribute("aimode"));
	}
}



//-----------------------------------------------------------------------------
//  ID
//-----------------------------------------------------------------------------
void SetID(CObject3D *Object, TiXmlElement *Element)
{
	if (Element->Attribute("id"))
		Object->ID = atoi(Element->Attribute("id"));
}



//-----------------------------------------------------------------------------
// Location
//-----------------------------------------------------------------------------
void SetShipLocation(CSpaceShip *Object, TiXmlElement *Element, float TimeOpLag)
{
	VECTOR3D POS(0.0f, 0.0f, 0.0f);

	// абсолютные координаты
	if (Element->Attribute("posax"))
		POS.x = (float)atof(Element->Attribute("posax"));
	if (Element->Attribute("posay"))
		POS.y = (float)atof(Element->Attribute("posay"));
	if (Element->Attribute("posaz"))
		POS.z = (float)atof(Element->Attribute("posaz"));

	// относительные координаты
	VECTOR3D PosWithLag(0.0f,0.0f,0.0f);
	// находим на сколько перелетим
	PosWithLag = GameCameraMovement^(-GameCameraGetSpeed()*TimeOpLag);
	if (Element->Attribute("posx"))
		POS.x = (float)atof(Element->Attribute("posx")) + GamePoint.x + PosWithLag.x;
	if (Element->Attribute("posy"))
		POS.y = (float)atof(Element->Attribute("posy")) + GamePoint.y + PosWithLag.y;
	if (Element->Attribute("posz"))
		POS.z = (float)atof(Element->Attribute("posz")) + GamePoint.z + PosWithLag.z;

	Object->SetLocation(POS);
}
void SetProjectileLocation(CProjectile *Object, TiXmlElement *Element, float TimeOpLag)
{
	VECTOR3D POS(0.0f, 0.0f, 0.0f);

	// абсолютные координаты
	if (Element->Attribute("posax"))
		POS.x = (float)atof(Element->Attribute("posax"));
	if (Element->Attribute("posay"))
		POS.y = (float)atof(Element->Attribute("posay"));
	if (Element->Attribute("posaz"))
		POS.z = (float)atof(Element->Attribute("posaz"));

	// относительные координаты
	VECTOR3D PosWithLag(0.0f,0.0f,0.0f);
	// находим на сколько перелетим
	PosWithLag = GameCameraMovement^(-GameCameraGetSpeed()*TimeOpLag);
	if (Element->Attribute("posx"))
		POS.x = (float)atof(Element->Attribute("posx")) + GamePoint.x + PosWithLag.x;
	if (Element->Attribute("posy"))
		POS.y = (float)atof(Element->Attribute("posy")) + GamePoint.y + PosWithLag.y;
	if (Element->Attribute("posz"))
		POS.z = (float)atof(Element->Attribute("posz")) + GamePoint.z + PosWithLag.z;

	Object->SetLocation(POS);
}
void SetLocation(CObject3D *Object, TiXmlElement *Element, float TimeOpLag)
{
	VECTOR3D POS(0.0f, 0.0f, 0.0f);

	// абсолютные координаты
	if (Element->Attribute("posax"))
		POS.x = (float)atof(Element->Attribute("posax"));
	if (Element->Attribute("posay"))
		POS.y = (float)atof(Element->Attribute("posay"));
	if (Element->Attribute("posaz"))
		POS.z = (float)atof(Element->Attribute("posaz"));

	// относительные координаты
	VECTOR3D PosWithLag(0.0f,0.0f,0.0f);
	// находим на сколько перелетим
	PosWithLag = GameCameraMovement^(-GameCameraGetSpeed()*TimeOpLag);
	if (Element->Attribute("posx"))
		POS.x = (float)atof(Element->Attribute("posx")) + GamePoint.x + PosWithLag.x;
	if (Element->Attribute("posy"))
		POS.y = (float)atof(Element->Attribute("posy")) + GamePoint.y + PosWithLag.y;
	if (Element->Attribute("posz"))
		POS.z = (float)atof(Element->Attribute("posz")) + GamePoint.z + PosWithLag.z;

	Object->SetLocation(POS);
}





//-----------------------------------------------------------------------------
// Rotation
//-----------------------------------------------------------------------------
void SetShipRotation(CSpaceShip *Object, TiXmlElement *Element)
{
	VECTOR3D ANGLE(0.0f, 0.0f, 0.0f);
	if (Element->Attribute("anglax"))
		ANGLE.x = (float)atof(Element->Attribute("anglax"));
	if (Element->Attribute("anglay"))
		ANGLE.y = (float)atof(Element->Attribute("anglay"));
	if (Element->Attribute("anglaz"))
		ANGLE.z = (float)atof(Element->Attribute("anglaz"));

// пока делает тоже самое!!! потом переделать
	if (Element->Attribute("anglx"))
		ANGLE.x = (float)atof(Element->Attribute("anglx"));
	if (Element->Attribute("angly"))
		ANGLE.y = (float)atof(Element->Attribute("angly"));
	if (Element->Attribute("anglz"))
		ANGLE.z = (float)atof(Element->Attribute("anglz"));

	Object->SetRotation(ANGLE);
}
void SetProjectileRotation(CProjectile *Object, TiXmlElement *Element)
{
	VECTOR3D ANGLE(0.0f, 0.0f, 0.0f);
	if (Element->Attribute("anglax"))
		ANGLE.x = (float)atof(Element->Attribute("anglax"));
	if (Element->Attribute("anglay"))
		ANGLE.y = (float)atof(Element->Attribute("anglay"));
	if (Element->Attribute("anglaz"))
		ANGLE.z = (float)atof(Element->Attribute("anglaz"));

// пока делает тоже самое!!! потом переделать
	if (Element->Attribute("anglx"))
		ANGLE.x = (float)atof(Element->Attribute("anglx"));
	if (Element->Attribute("angly"))
		ANGLE.y = (float)atof(Element->Attribute("angly"));
	if (Element->Attribute("anglz"))
		ANGLE.z = (float)atof(Element->Attribute("anglz"));

	Object->SetRotation(ANGLE);
}
void SetRotation(CObject3D *Object, TiXmlElement *Element)
{
	VECTOR3D ANGLE(0.0f, 0.0f, 0.0f);
	if (Element->Attribute("anglax"))
		ANGLE.x = (float)atof(Element->Attribute("anglax"));
	if (Element->Attribute("anglay"))
		ANGLE.y = (float)atof(Element->Attribute("anglay"));
	if (Element->Attribute("anglaz"))
		ANGLE.z = (float)atof(Element->Attribute("anglaz"));

// пока делает тоже самое!!! потом переделать
	if (Element->Attribute("anglx"))
		ANGLE.x = (float)atof(Element->Attribute("anglx"));
	if (Element->Attribute("angly"))
		ANGLE.y = (float)atof(Element->Attribute("angly"));
	if (Element->Attribute("anglz"))
		ANGLE.z = (float)atof(Element->Attribute("anglz"));

	Object->SetRotation(ANGLE);
}




//-----------------------------------------------------------------------------
// DeleteOnHide
//-----------------------------------------------------------------------------
void SetShowDeleteOnHide(CObject3D *Object, TiXmlElement *Element)
{
	Object->ShowDeleteOnHide = 0;
	if (Element->Attribute("onhide"))
		if (atoi(Element->Attribute("onhide")) <= 0) Object->ShowDeleteOnHide = -1;
}




//-----------------------------------------------------------------------------
// DebugInformation
//-----------------------------------------------------------------------------
void SetDebugInformation(CObject3D *Object, TiXmlElement *Element)
{
	// не нужно ничего устанавливать, выходим
	if (!Script->ShowDebugModeLine) return;

	// преобразовываем число в строку
	char buffer[10];
	itoa10(Element->Row(), buffer);

	Object->DebugInfo = new char[strlen(buffer)+1];
	strcpy(Object->DebugInfo, buffer);
}
