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


#ifndef PIRATESHIP_H
#define PIRATESHIP_H


#include "../SpaceShip.h"



//-----------------------------------------------------------------------------
// Класс CPirateShip
//-----------------------------------------------------------------------------
class CPirateShip : public CSpaceShip
{
public:

	virtual ~CPirateShip(){};

	// Создание нужного объекта
	virtual void	Create(int	PirateShipNum);

};




//-----------------------------------------------------------------------------
// Дополнительные функции для CPirateShip
//-----------------------------------------------------------------------------

// Создание двигателя
void SetPirateShipEngine(eParticleSystem *ParticleSystem, int EngineType);




#endif // PIRATESHIP_H
