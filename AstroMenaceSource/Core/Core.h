/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Core.h

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.0

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

#ifndef Core_H
#define Core_H


#include "Base.h"


#include "Math/Math.h"
#include "Texture/Texture.h"
#include "Light/Light.h"
#include "Font/Font.h"
#include "ParticleSystem/ParticleSystem.h"
#include "ParticleSystem2D/ParticleSystem2D.h"
#include "System/System.h"
#include "CollisionDetection/CollisionDetection.h"
#include "VirtualFileSystem/VFS.h"
#include "RendererInterface/RendererInterface.h"
#include "Sound/Sound.h"
#include "Camera/Camera.h"
#include "Model3D/Model3D.h"

#ifdef buildin_tynixml
	#include "Parser/TinyXML/tinyxml.h"
#else
	#include "tinyxml.h"
#endif // buildin_tynixml


#endif // Core_H
