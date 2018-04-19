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
	int tmpAI_Mode{0};
	if (xmlDoc->iGetEntryAttribute(*xmlEntry, "aimode", tmpAI_Mode)) {
		// собираем новый элемент
		sTimeSheet *TimeSheet;
		TimeSheet = new sTimeSheet;
		Object->AttachTimeSheet(TimeSheet);

		TimeSheet->Time = -1;
		TimeSheet->AI_Mode = tmpAI_Mode;
	}
}



//-----------------------------------------------------------------------------
//  ID
//-----------------------------------------------------------------------------
void SetID(cObject3D *Object, sXMLEntry *xmlEntry, cXMLDocument *xmlDoc)
{
	xmlDoc->iGetEntryAttribute(*xmlEntry, "id", Object->ID);
}



//-----------------------------------------------------------------------------
// Location
//-----------------------------------------------------------------------------
void SetShipLocation(cSpaceShip *Object, sXMLEntry *xmlEntry, cXMLDocument *xmlDoc, float TimeOpLag)
{
	sVECTOR3D POS(0.0f, 0.0f, 0.0f);

	// абсолютные координаты
	xmlDoc->fGetEntryAttribute(*xmlEntry, "posax", POS.x);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "posay", POS.y);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "posaz", POS.z);

	// относительные координаты
	sVECTOR3D PosWithLag(0.0f,0.0f,0.0f);
	// находим на сколько перелетим
	PosWithLag = GameCameraMovement^(-GameCameraGetSpeed()*TimeOpLag);
	if (xmlDoc->fGetEntryAttribute(*xmlEntry, "posx", POS.x))
		POS.x += GamePoint.x + PosWithLag.x;
	if (xmlDoc->fGetEntryAttribute(*xmlEntry, "posy", POS.y))
		POS.y += GamePoint.y + PosWithLag.y;
	if (xmlDoc->fGetEntryAttribute(*xmlEntry, "posz", POS.z))
		POS.z += GamePoint.z + PosWithLag.z;

	Object->SetLocation(POS);
}
void SetProjectileLocation(cProjectile *Object, sXMLEntry *xmlEntry, cXMLDocument *xmlDoc, float TimeOpLag)
{
	sVECTOR3D POS(0.0f, 0.0f, 0.0f);

	// абсолютные координаты
	xmlDoc->fGetEntryAttribute(*xmlEntry, "posax", POS.x);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "posay", POS.y);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "posaz", POS.z);

	// относительные координаты
	sVECTOR3D PosWithLag(0.0f,0.0f,0.0f);
	// находим на сколько перелетим
	PosWithLag = GameCameraMovement^(-GameCameraGetSpeed()*TimeOpLag);
	if (xmlDoc->fGetEntryAttribute(*xmlEntry, "posx", POS.x))
		POS.x += GamePoint.x + PosWithLag.x;
	if (xmlDoc->fGetEntryAttribute(*xmlEntry, "posy", POS.y))
		POS.y += GamePoint.y + PosWithLag.y;
	if (xmlDoc->fGetEntryAttribute(*xmlEntry, "posz", POS.z))
		POS.z += GamePoint.z + PosWithLag.z;

	Object->SetLocation(POS);
}
void SetLocation(cObject3D *Object, sXMLEntry *xmlEntry, cXMLDocument *xmlDoc, float TimeOpLag)
{
	sVECTOR3D POS(0.0f, 0.0f, 0.0f);

	// абсолютные координаты
	xmlDoc->fGetEntryAttribute(*xmlEntry, "posax", POS.x);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "posay", POS.y);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "posaz", POS.z);

	// относительные координаты
	sVECTOR3D PosWithLag(0.0f,0.0f,0.0f);
	// находим на сколько перелетим
	PosWithLag = GameCameraMovement^(-GameCameraGetSpeed()*TimeOpLag);
	if (xmlDoc->fGetEntryAttribute(*xmlEntry, "posx", POS.x))
		POS.x += GamePoint.x + PosWithLag.x;
	if (xmlDoc->fGetEntryAttribute(*xmlEntry, "posy", POS.y))
		POS.y += GamePoint.y + PosWithLag.y;
	if (xmlDoc->fGetEntryAttribute(*xmlEntry, "posz", POS.z))
		POS.z += GamePoint.z + PosWithLag.z;

	Object->SetLocation(POS);
}





//-----------------------------------------------------------------------------
// Rotation
//-----------------------------------------------------------------------------
void SetShipRotation(cSpaceShip *Object, sXMLEntry *xmlEntry, cXMLDocument *xmlDoc)
{
	sVECTOR3D ANGLE(0.0f, 0.0f, 0.0f);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "anglax", ANGLE.x);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "anglay", ANGLE.y);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "anglaz", ANGLE.z);

// пока делает тоже самое!!! потом переделать
	xmlDoc->fGetEntryAttribute(*xmlEntry, "anglx", ANGLE.x);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "angly", ANGLE.y);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "anglz", ANGLE.z);

	Object->SetRotation(ANGLE);
}
void SetProjectileRotation(cProjectile *Object, sXMLEntry *xmlEntry, cXMLDocument *xmlDoc)
{
	sVECTOR3D ANGLE(0.0f, 0.0f, 0.0f);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "anglax", ANGLE.x);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "anglay", ANGLE.y);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "anglaz", ANGLE.z);

// пока делает тоже самое!!! потом переделать
	xmlDoc->fGetEntryAttribute(*xmlEntry, "anglx", ANGLE.x);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "angly", ANGLE.y);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "anglz", ANGLE.z);

	Object->SetRotation(ANGLE);
}
void SetRotation(cObject3D *Object, sXMLEntry *xmlEntry, cXMLDocument *xmlDoc)
{
	sVECTOR3D ANGLE(0.0f, 0.0f, 0.0f);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "anglax", ANGLE.x);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "anglay", ANGLE.y);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "anglaz" ,ANGLE.z);

// пока делает тоже самое!!! потом переделать
	xmlDoc->fGetEntryAttribute(*xmlEntry, "anglx", ANGLE.x);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "angly", ANGLE.y);
	xmlDoc->fGetEntryAttribute(*xmlEntry, "anglz", ANGLE.z);

	Object->SetRotation(ANGLE);
}




//-----------------------------------------------------------------------------
// DeleteOnHide
//-----------------------------------------------------------------------------
void SetShowDeleteOnHide(cObject3D *Object, sXMLEntry *xmlEntry, cXMLDocument *xmlDoc)
{
	Object->ShowDeleteOnHide = 0;
	if (xmlDoc->iGetEntryAttribute(*xmlEntry, "onhide", Object->ShowDeleteOnHide) &&
	    (Object->ShowDeleteOnHide <= 0))
			Object->ShowDeleteOnHide = -1;
}




//-----------------------------------------------------------------------------
// DebugInformation
//-----------------------------------------------------------------------------
#ifdef NDEBUG
void SetDebugInformation(cObject3D *UNUSED(Object), sXMLEntry *UNUSED(xmlEntry))
{
	// не нужно ничего устанавливать, выходим
	if (!Script->ShowDebugModeLine)
		return;

	return;
}
#else
void SetDebugInformation(cObject3D *Object, sXMLEntry *xmlEntry)
{
	// не нужно ничего устанавливать, выходим
	if (!Script->ShowDebugModeLine)
		return;

	std::string buffer{std::to_string(xmlEntry->LineNumber)};

	Object->DebugInfo = new char[buffer.size()+1];
	strcpy(Object->DebugInfo, buffer.c_str());
}
#endif // NDEBUG
