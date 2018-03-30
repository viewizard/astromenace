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
#include "script.h"
#include "../object3d/object3d.h"
#include "../object3d/space_ship/space_ship.h"



//-----------------------------------------------------------------------------
// aimode
//-----------------------------------------------------------------------------
void SetAIMode(cObject3D *Object, sXMLEntry *xmlEntry, cXMLDocument *xmlDoc)
{
	if (xmlDoc->TestEntryAttribute(xmlEntry, "aimode")) {
		// собираем новый элемент
		sTimeSheet *TimeSheet;
		TimeSheet = new sTimeSheet;
		Object->AttachTimeSheet(TimeSheet);

		TimeSheet->Time = -1;
		TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(xmlEntry, "aimode");
	}
}



//-----------------------------------------------------------------------------
//  ID
//-----------------------------------------------------------------------------
void SetID(cObject3D *Object, sXMLEntry *xmlEntry, cXMLDocument *xmlDoc)
{
	if (xmlDoc->TestEntryAttribute(xmlEntry, "id"))
		Object->ID = xmlDoc->iGetEntryAttribute(xmlEntry, "id");
}



//-----------------------------------------------------------------------------
// Location
//-----------------------------------------------------------------------------
void SetShipLocation(cSpaceShip *Object, sXMLEntry *xmlEntry, cXMLDocument *xmlDoc, float TimeOpLag)
{
	sVECTOR3D POS(0.0f, 0.0f, 0.0f);

	// абсолютные координаты
	if (xmlDoc->TestEntryAttribute(xmlEntry, "posax"))
		POS.x = xmlDoc->fGetEntryAttribute(xmlEntry, "posax");
	if (xmlDoc->TestEntryAttribute(xmlEntry, "posay"))
		POS.y = xmlDoc->fGetEntryAttribute(xmlEntry, "posay");
	if (xmlDoc->TestEntryAttribute(xmlEntry, "posaz"))
		POS.z = xmlDoc->fGetEntryAttribute(xmlEntry, "posaz");

	// относительные координаты
	sVECTOR3D PosWithLag(0.0f,0.0f,0.0f);
	// находим на сколько перелетим
	PosWithLag = GameCameraMovement^(-GameCameraGetSpeed()*TimeOpLag);
	if (xmlDoc->TestEntryAttribute(xmlEntry, "posx"))
		POS.x = xmlDoc->fGetEntryAttribute(xmlEntry, "posx") + GamePoint.x + PosWithLag.x;
	if (xmlDoc->TestEntryAttribute(xmlEntry, "posy"))
		POS.y = xmlDoc->fGetEntryAttribute(xmlEntry, "posy") + GamePoint.y + PosWithLag.y;
	if (xmlDoc->TestEntryAttribute(xmlEntry, "posz"))
		POS.z = xmlDoc->fGetEntryAttribute(xmlEntry, "posz") + GamePoint.z + PosWithLag.z;

	Object->SetLocation(POS);
}
void SetProjectileLocation(cProjectile *Object, sXMLEntry *xmlEntry, cXMLDocument *xmlDoc, float TimeOpLag)
{
	sVECTOR3D POS(0.0f, 0.0f, 0.0f);

	// абсолютные координаты
	if (xmlDoc->TestEntryAttribute(xmlEntry, "posax"))
		POS.x = xmlDoc->fGetEntryAttribute(xmlEntry, "posax");
	if (xmlDoc->TestEntryAttribute(xmlEntry, "posay"))
		POS.y = xmlDoc->fGetEntryAttribute(xmlEntry, "posay");
	if (xmlDoc->TestEntryAttribute(xmlEntry, "posaz"))
		POS.z = xmlDoc->fGetEntryAttribute(xmlEntry, "posaz");

	// относительные координаты
	sVECTOR3D PosWithLag(0.0f,0.0f,0.0f);
	// находим на сколько перелетим
	PosWithLag = GameCameraMovement^(-GameCameraGetSpeed()*TimeOpLag);
	if (xmlDoc->TestEntryAttribute(xmlEntry, "posx"))
		POS.x = xmlDoc->fGetEntryAttribute(xmlEntry, "posx") + GamePoint.x + PosWithLag.x;
	if (xmlDoc->TestEntryAttribute(xmlEntry, "posy"))
		POS.y = xmlDoc->fGetEntryAttribute(xmlEntry, "posy") + GamePoint.y + PosWithLag.y;
	if (xmlDoc->TestEntryAttribute(xmlEntry, "posz"))
		POS.z = xmlDoc->fGetEntryAttribute(xmlEntry, "posz") + GamePoint.z + PosWithLag.z;

	Object->SetLocation(POS);
}
void SetLocation(cObject3D *Object, sXMLEntry *xmlEntry, cXMLDocument *xmlDoc, float TimeOpLag)
{
	sVECTOR3D POS(0.0f, 0.0f, 0.0f);

	// абсолютные координаты
	if (xmlDoc->TestEntryAttribute(xmlEntry, "posax"))
		POS.x = xmlDoc->fGetEntryAttribute(xmlEntry, "posax");
	if (xmlDoc->TestEntryAttribute(xmlEntry, "posay"))
		POS.y = xmlDoc->fGetEntryAttribute(xmlEntry, "posay");
	if (xmlDoc->TestEntryAttribute(xmlEntry, "posaz"))
		POS.z = xmlDoc->fGetEntryAttribute(xmlEntry, "posaz");

	// относительные координаты
	sVECTOR3D PosWithLag(0.0f,0.0f,0.0f);
	// находим на сколько перелетим
	PosWithLag = GameCameraMovement^(-GameCameraGetSpeed()*TimeOpLag);
	if (xmlDoc->TestEntryAttribute(xmlEntry, "posx"))
		POS.x = xmlDoc->fGetEntryAttribute(xmlEntry, "posx") + GamePoint.x + PosWithLag.x;
	if (xmlDoc->TestEntryAttribute(xmlEntry, "posy"))
		POS.y = xmlDoc->fGetEntryAttribute(xmlEntry, "posy") + GamePoint.y + PosWithLag.y;
	if (xmlDoc->TestEntryAttribute(xmlEntry, "posz"))
		POS.z = xmlDoc->fGetEntryAttribute(xmlEntry, "posz") + GamePoint.z + PosWithLag.z;

	Object->SetLocation(POS);
}





//-----------------------------------------------------------------------------
// Rotation
//-----------------------------------------------------------------------------
void SetShipRotation(cSpaceShip *Object, sXMLEntry *xmlEntry, cXMLDocument *xmlDoc)
{
	sVECTOR3D ANGLE(0.0f, 0.0f, 0.0f);
	if (xmlDoc->TestEntryAttribute(xmlEntry, "anglax"))
		ANGLE.x = xmlDoc->fGetEntryAttribute(xmlEntry, "anglax");
	if (xmlDoc->TestEntryAttribute(xmlEntry, "anglay"))
		ANGLE.y = xmlDoc->fGetEntryAttribute(xmlEntry, "anglay");
	if (xmlDoc->TestEntryAttribute(xmlEntry, "anglaz"))
		ANGLE.z = xmlDoc->fGetEntryAttribute(xmlEntry, "anglaz");

// пока делает тоже самое!!! потом переделать
	if (xmlDoc->TestEntryAttribute(xmlEntry, "anglx"))
		ANGLE.x = xmlDoc->fGetEntryAttribute(xmlEntry, "anglx");
	if (xmlDoc->TestEntryAttribute(xmlEntry, "angly"))
		ANGLE.y = xmlDoc->fGetEntryAttribute(xmlEntry, "angly");
	if (xmlDoc->TestEntryAttribute(xmlEntry, "anglz"))
		ANGLE.z = xmlDoc->fGetEntryAttribute(xmlEntry, "anglz");

	Object->SetRotation(ANGLE);
}
void SetProjectileRotation(cProjectile *Object, sXMLEntry *xmlEntry, cXMLDocument *xmlDoc)
{
	sVECTOR3D ANGLE(0.0f, 0.0f, 0.0f);
	if (xmlDoc->TestEntryAttribute(xmlEntry, "anglax"))
		ANGLE.x = xmlDoc->fGetEntryAttribute(xmlEntry, "anglax");
	if (xmlDoc->TestEntryAttribute(xmlEntry, "anglay"))
		ANGLE.y = xmlDoc->fGetEntryAttribute(xmlEntry, "anglay");
	if (xmlDoc->TestEntryAttribute(xmlEntry, "anglaz"))
		ANGLE.z = xmlDoc->fGetEntryAttribute(xmlEntry, "anglaz");

// пока делает тоже самое!!! потом переделать
	if (xmlDoc->TestEntryAttribute(xmlEntry, "anglx"))
		ANGLE.x = xmlDoc->fGetEntryAttribute(xmlEntry, "anglx");
	if (xmlDoc->TestEntryAttribute(xmlEntry, "angly"))
		ANGLE.y = xmlDoc->fGetEntryAttribute(xmlEntry, "angly");
	if (xmlDoc->TestEntryAttribute(xmlEntry, "anglz"))
		ANGLE.z = xmlDoc->fGetEntryAttribute(xmlEntry, "anglz");

	Object->SetRotation(ANGLE);
}
void SetRotation(cObject3D *Object, sXMLEntry *xmlEntry, cXMLDocument *xmlDoc)
{
	sVECTOR3D ANGLE(0.0f, 0.0f, 0.0f);
	if (xmlDoc->TestEntryAttribute(xmlEntry, "anglax"))
		ANGLE.x = xmlDoc->fGetEntryAttribute(xmlEntry, "anglax");
	if (xmlDoc->TestEntryAttribute(xmlEntry, "anglay"))
		ANGLE.y = xmlDoc->fGetEntryAttribute(xmlEntry, "anglay");
	if (xmlDoc->TestEntryAttribute(xmlEntry, "anglaz"))
		ANGLE.z = xmlDoc->fGetEntryAttribute(xmlEntry, "anglaz");

// пока делает тоже самое!!! потом переделать
	if (xmlDoc->TestEntryAttribute(xmlEntry, "anglx"))
		ANGLE.x = xmlDoc->fGetEntryAttribute(xmlEntry, "anglx");
	if (xmlDoc->TestEntryAttribute(xmlEntry, "angly"))
		ANGLE.y = xmlDoc->fGetEntryAttribute(xmlEntry, "angly");
	if (xmlDoc->TestEntryAttribute(xmlEntry, "anglz"))
		ANGLE.z = xmlDoc->fGetEntryAttribute(xmlEntry, "anglz");

	Object->SetRotation(ANGLE);
}




//-----------------------------------------------------------------------------
// DeleteOnHide
//-----------------------------------------------------------------------------
void SetShowDeleteOnHide(cObject3D *Object, sXMLEntry *xmlEntry, cXMLDocument *xmlDoc)
{
	Object->ShowDeleteOnHide = 0;
	if (xmlDoc->TestEntryAttribute(xmlEntry, "onhide"))
		if (xmlDoc->iGetEntryAttribute(xmlEntry, "onhide") <= 0) Object->ShowDeleteOnHide = -1;
}




//-----------------------------------------------------------------------------
// DebugInformation
//-----------------------------------------------------------------------------
#ifndef gamedebug
void SetDebugInformation(cObject3D *UNUSED(Object), sXMLEntry *UNUSED(xmlEntry))
#else
void SetDebugInformation(cObject3D *Object, sXMLEntry *xmlEntry)
#endif //gamedebug
{
	// не нужно ничего устанавливать, выходим
	if (!Script->ShowDebugModeLine) return;

#ifndef gamedebug
	std::cerr << __func__ << "(): " << "XML don't count lines, uncomment \"#define gamedebug\"\n"
		  << "line in config.h file and re-compile game first.\n";
	return;
#else
	// преобразовываем число в строку
	std::string buffer{std::to_string(xmlEntry->LineNumber)};

	Object->DebugInfo = new char[buffer.size()+1];
	strcpy(Object->DebugInfo, buffer.c_str());
#endif //gamedebug
}
