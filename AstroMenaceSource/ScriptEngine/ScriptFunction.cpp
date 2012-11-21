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
