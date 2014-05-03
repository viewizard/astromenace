/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2014 Michael Kurinnoy, Viewizard


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




//-----------------------------------------------------------------------------
// aimode
//-----------------------------------------------------------------------------
void SetAIMode(CObject3D *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc)
{
	if (xmlDoc->GetEntryAttribute(xmlEntry, "aimode") != 0)
	{
		// собираем новый элемент
		CTimeSheet *TimeSheet;
		TimeSheet = new CTimeSheet;
		Object->AttachTimeSheet(TimeSheet);

		TimeSheet->Time = -1;
		TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(xmlEntry, "aimode");
	}
}



//-----------------------------------------------------------------------------
//  ID
//-----------------------------------------------------------------------------
void SetID(CObject3D *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc)
{
	if (xmlDoc->GetEntryAttribute(xmlEntry, "id") != 0)
		Object->ID = xmlDoc->iGetEntryAttribute(xmlEntry, "id");
}



//-----------------------------------------------------------------------------
// Location
//-----------------------------------------------------------------------------
void SetShipLocation(CSpaceShip *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc, float TimeOpLag)
{
	VECTOR3D POS(0.0f, 0.0f, 0.0f);

	// абсолютные координаты
	if (xmlDoc->GetEntryAttribute(xmlEntry, "posax") != 0)
		POS.x = xmlDoc->fGetEntryAttribute(xmlEntry, "posax");
	if (xmlDoc->GetEntryAttribute(xmlEntry, "posay") != 0)
		POS.y = xmlDoc->fGetEntryAttribute(xmlEntry, "posay");
	if (xmlDoc->GetEntryAttribute(xmlEntry, "posaz") != 0)
		POS.z = xmlDoc->fGetEntryAttribute(xmlEntry, "posaz");

	// относительные координаты
	VECTOR3D PosWithLag(0.0f,0.0f,0.0f);
	// находим на сколько перелетим
	PosWithLag = GameCameraMovement^(-GameCameraGetSpeed()*TimeOpLag);
	if (xmlDoc->GetEntryAttribute(xmlEntry, "posx") != 0)
		POS.x = xmlDoc->fGetEntryAttribute(xmlEntry, "posx") + GamePoint.x + PosWithLag.x;
	if (xmlDoc->GetEntryAttribute(xmlEntry, "posy") != 0)
		POS.y = xmlDoc->fGetEntryAttribute(xmlEntry, "posy") + GamePoint.y + PosWithLag.y;
	if (xmlDoc->GetEntryAttribute(xmlEntry, "posz") != 0)
		POS.z = xmlDoc->fGetEntryAttribute(xmlEntry, "posz") + GamePoint.z + PosWithLag.z;

	Object->SetLocation(POS);
}
void SetProjectileLocation(CProjectile *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc, float TimeOpLag)
{
	VECTOR3D POS(0.0f, 0.0f, 0.0f);

	// абсолютные координаты
	if (xmlDoc->GetEntryAttribute(xmlEntry, "posax") != 0)
		POS.x = xmlDoc->fGetEntryAttribute(xmlEntry, "posax");
	if (xmlDoc->GetEntryAttribute(xmlEntry, "posay") != 0)
		POS.y = xmlDoc->fGetEntryAttribute(xmlEntry, "posay");
	if (xmlDoc->GetEntryAttribute(xmlEntry, "posaz") != 0)
		POS.z = xmlDoc->fGetEntryAttribute(xmlEntry, "posaz");

	// относительные координаты
	VECTOR3D PosWithLag(0.0f,0.0f,0.0f);
	// находим на сколько перелетим
	PosWithLag = GameCameraMovement^(-GameCameraGetSpeed()*TimeOpLag);
	if (xmlDoc->GetEntryAttribute(xmlEntry, "posx") != 0)
		POS.x = xmlDoc->fGetEntryAttribute(xmlEntry, "posx") + GamePoint.x + PosWithLag.x;
	if (xmlDoc->GetEntryAttribute(xmlEntry, "posy") != 0)
		POS.y = xmlDoc->fGetEntryAttribute(xmlEntry, "posy") + GamePoint.y + PosWithLag.y;
	if (xmlDoc->GetEntryAttribute(xmlEntry, "posz") != 0)
		POS.z = xmlDoc->fGetEntryAttribute(xmlEntry, "posz") + GamePoint.z + PosWithLag.z;

	Object->SetLocation(POS);
}
void SetLocation(CObject3D *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc, float TimeOpLag)
{
	VECTOR3D POS(0.0f, 0.0f, 0.0f);

	// абсолютные координаты
	if (xmlDoc->GetEntryAttribute(xmlEntry, "posax") != 0)
		POS.x = xmlDoc->fGetEntryAttribute(xmlEntry, "posax");
	if (xmlDoc->GetEntryAttribute(xmlEntry, "posay") != 0)
		POS.y = xmlDoc->fGetEntryAttribute(xmlEntry, "posay");
	if (xmlDoc->GetEntryAttribute(xmlEntry, "posaz") != 0)
		POS.z = xmlDoc->fGetEntryAttribute(xmlEntry, "posaz");

	// относительные координаты
	VECTOR3D PosWithLag(0.0f,0.0f,0.0f);
	// находим на сколько перелетим
	PosWithLag = GameCameraMovement^(-GameCameraGetSpeed()*TimeOpLag);
	if (xmlDoc->GetEntryAttribute(xmlEntry, "posx") != 0)
		POS.x = xmlDoc->fGetEntryAttribute(xmlEntry, "posx") + GamePoint.x + PosWithLag.x;
	if (xmlDoc->GetEntryAttribute(xmlEntry, "posy") != 0)
		POS.y = xmlDoc->fGetEntryAttribute(xmlEntry, "posy") + GamePoint.y + PosWithLag.y;
	if (xmlDoc->GetEntryAttribute(xmlEntry, "posz") != 0)
		POS.z = xmlDoc->fGetEntryAttribute(xmlEntry, "posz") + GamePoint.z + PosWithLag.z;

	Object->SetLocation(POS);
}





//-----------------------------------------------------------------------------
// Rotation
//-----------------------------------------------------------------------------
void SetShipRotation(CSpaceShip *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc)
{
	VECTOR3D ANGLE(0.0f, 0.0f, 0.0f);
	if (xmlDoc->GetEntryAttribute(xmlEntry, "anglax") != 0)
		ANGLE.x = xmlDoc->fGetEntryAttribute(xmlEntry, "anglax");
	if (xmlDoc->GetEntryAttribute(xmlEntry, "anglay") != 0)
		ANGLE.y = xmlDoc->fGetEntryAttribute(xmlEntry, "anglay");
	if (xmlDoc->GetEntryAttribute(xmlEntry, "anglaz") != 0)
		ANGLE.z = xmlDoc->fGetEntryAttribute(xmlEntry, "anglaz");

// пока делает тоже самое!!! потом переделать
	if (xmlDoc->GetEntryAttribute(xmlEntry, "anglx") != 0)
		ANGLE.x = xmlDoc->fGetEntryAttribute(xmlEntry, "anglx");
	if (xmlDoc->GetEntryAttribute(xmlEntry, "angly") != 0)
		ANGLE.y = xmlDoc->fGetEntryAttribute(xmlEntry, "angly");
	if (xmlDoc->GetEntryAttribute(xmlEntry, "anglz") != 0)
		ANGLE.z = xmlDoc->fGetEntryAttribute(xmlEntry, "anglz");

	Object->SetRotation(ANGLE);
}
void SetProjectileRotation(CProjectile *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc)
{
	VECTOR3D ANGLE(0.0f, 0.0f, 0.0f);
	if (xmlDoc->GetEntryAttribute(xmlEntry, "anglax") != 0)
		ANGLE.x = xmlDoc->fGetEntryAttribute(xmlEntry, "anglax");
	if (xmlDoc->GetEntryAttribute(xmlEntry, "anglay") != 0)
		ANGLE.y = xmlDoc->fGetEntryAttribute(xmlEntry, "anglay");
	if (xmlDoc->GetEntryAttribute(xmlEntry, "anglaz") != 0)
		ANGLE.z = xmlDoc->fGetEntryAttribute(xmlEntry, "anglaz");

// пока делает тоже самое!!! потом переделать
	if (xmlDoc->GetEntryAttribute(xmlEntry, "anglx") != 0)
		ANGLE.x = xmlDoc->fGetEntryAttribute(xmlEntry, "anglx");
	if (xmlDoc->GetEntryAttribute(xmlEntry, "angly") != 0)
		ANGLE.y = xmlDoc->fGetEntryAttribute(xmlEntry, "angly");
	if (xmlDoc->GetEntryAttribute(xmlEntry, "anglz") != 0)
		ANGLE.z = xmlDoc->fGetEntryAttribute(xmlEntry, "anglz");

	Object->SetRotation(ANGLE);
}
void SetRotation(CObject3D *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc)
{
	VECTOR3D ANGLE(0.0f, 0.0f, 0.0f);
	if (xmlDoc->GetEntryAttribute(xmlEntry, "anglax") != 0)
		ANGLE.x = xmlDoc->fGetEntryAttribute(xmlEntry, "anglax");
	if (xmlDoc->GetEntryAttribute(xmlEntry, "anglay") != 0)
		ANGLE.y = xmlDoc->fGetEntryAttribute(xmlEntry, "anglay");
	if (xmlDoc->GetEntryAttribute(xmlEntry, "anglaz") != 0)
		ANGLE.z = xmlDoc->fGetEntryAttribute(xmlEntry, "anglaz");

// пока делает тоже самое!!! потом переделать
	if (xmlDoc->GetEntryAttribute(xmlEntry, "anglx") != 0)
		ANGLE.x = xmlDoc->fGetEntryAttribute(xmlEntry, "anglx");
	if (xmlDoc->GetEntryAttribute(xmlEntry, "angly") != 0)
		ANGLE.y = xmlDoc->fGetEntryAttribute(xmlEntry, "angly");
	if (xmlDoc->GetEntryAttribute(xmlEntry, "anglz") != 0)
		ANGLE.z = xmlDoc->fGetEntryAttribute(xmlEntry, "anglz");

	Object->SetRotation(ANGLE);
}




//-----------------------------------------------------------------------------
// DeleteOnHide
//-----------------------------------------------------------------------------
void SetShowDeleteOnHide(CObject3D *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc)
{
	Object->ShowDeleteOnHide = 0;
	if (xmlDoc->GetEntryAttribute(xmlEntry, "onhide") != 0)
		if (xmlDoc->iGetEntryAttribute(xmlEntry, "onhide") <= 0) Object->ShowDeleteOnHide = -1;
}




//-----------------------------------------------------------------------------
// DebugInformation
//-----------------------------------------------------------------------------
#ifndef gamedebug
void SetDebugInformation(CObject3D *UNUSED(Object), cXMLEntry *UNUSED(xmlEntry))
#else
void SetDebugInformation(CObject3D *Object, cXMLEntry *xmlEntry)
#endif //gamedebug
{
	// не нужно ничего устанавливать, выходим
	if (!Script->ShowDebugModeLine) return;

#ifndef gamedebug
	printf("XML don't count lines, uncomment \"#define gamedebug\" line in config.h file and re-compile game first.\n");
	return;
#else
	// преобразовываем число в строку
	char buffer[10];
	itoa10(xmlEntry->LineNumber, buffer);

	Object->DebugInfo = new char[strlen(buffer)+1];
	strcpy(Object->DebugInfo, buffer);
#endif //gamedebug
}
