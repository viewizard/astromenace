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
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// TODO translate comments

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

#include "../core/core.h"
#include "../enum.h"
#include "../config/config.h"
#include "../assets/audio.h"
#include "../assets/texture.h"
#include "../ui/cursor.h"
#include "../command.h"
#include "../game.h" // FIXME "game.h" should be replaced by individual headers

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

struct cMission {
	std::string Title{};
	std::string Descr{};
	sRGBCOLOR TitleColor{sRGBCOLOR{eRGBCOLOR::orange}};
	sRGBCOLOR DescrColor{sRGBCOLOR{eRGBCOLOR::white}};
	std::string Icon{};
	std::string File{};
};

std::vector<cMission> MissionList{};

} // unnamed namespace



int SoundOnMissionID = -1;

int CurrentMission = -1;
// всего доступно миссий
int AllMission;
// начало и конец отображаемых миссий
int StartMission = 0;
int EndMission = 4;

bool SliderUnderMouseControl = false;



// получаем имя файла миссии
std::string GetCurrentMissionFileName()
{
	if ((CurrentMission >= 0) &&
	    (MissionList.size() > static_cast<unsigned>(CurrentMission)))
		return MissionList[CurrentMission].File;

	return std::string{};
}














//------------------------------------------------------------------------------------
// инициализация данных списка миссий
//------------------------------------------------------------------------------------
void MissionListInit()
{
	if (!MissionList.empty())
		return;

	std::string ScriptName{"script/list.xml"};

	// по скрипту, смотрим что загружать + считаем сколько позиций
	std::unique_ptr<cXMLDocument> xmlDoc{new cXMLDocument(ScriptName.c_str())};

	// проверяем корневой элемент
	if (!xmlDoc->GetRootEntry() || ("AstroMenaceMissionList" != xmlDoc->GetRootEntry()->Name)) {
		std::cerr << __func__ << "(): " << "Can't find AstroMenaceMissionList element in the: " << ScriptName << "\n";
		return;
	}

	for (const auto &xmlEntry : xmlDoc->GetRootEntry()->ChildrenList) {
		MissionList.emplace_back();

		// берем каждую миссию и смотрим настройки
		if (xmlEntry.Name == "Mission") {
			for (const auto &TMission : xmlEntry.ChildrenList) {
				// тайтл миссии
				if (TMission.Name == "Title") {
					int tmpColor{0};
					if (xmlDoc->iGetEntryAttribute(TMission, "color", tmpColor)) {
						switch (tmpColor) {
						case 1:
							MissionList.back().TitleColor = sRGBCOLOR{eRGBCOLOR::yellow};
							break;
						case 2:
							MissionList.back().TitleColor = sRGBCOLOR{eRGBCOLOR::red};
							break;
						case 3:
							MissionList.back().TitleColor = sRGBCOLOR{eRGBCOLOR::green};
							break;
						case 4:
							MissionList.back().TitleColor = sRGBCOLOR{eRGBCOLOR::orange};
							break;
						case 5: // grey
							MissionList.back().TitleColor = sRGBCOLOR{0.5f, 0.5f, 0.5f};
							break;
						case 6: // dark orange
							MissionList.back().TitleColor = sRGBCOLOR{1.0f, 0.3f, 0.0f};
							break;
						default:
							MissionList.back().TitleColor = sRGBCOLOR{eRGBCOLOR::white};
							break;
						}
					}
					MissionList.back().Title = TMission.Content;
				} else if (TMission.Name == "Descr") {
					int tmpColor{0};
					if (xmlDoc->iGetEntryAttribute(TMission, "color", tmpColor)) {
						switch (tmpColor) {
						case 1:
							MissionList.back().DescrColor = sRGBCOLOR{eRGBCOLOR::yellow};
							break;
						case 2:
							MissionList.back().DescrColor = sRGBCOLOR{eRGBCOLOR::red};
							break;
						case 3:
							MissionList.back().DescrColor = sRGBCOLOR{eRGBCOLOR::green};
							break;
						case 4:
							MissionList.back().DescrColor = sRGBCOLOR{eRGBCOLOR::orange};
							break;
						case 5: // grey
							MissionList.back().DescrColor = sRGBCOLOR{0.5f, 0.5f, 0.5f};
							break;
						case 6: // dark orange
							MissionList.back().DescrColor = sRGBCOLOR{1.0f, 0.3f, 0.0f};
							break;
						default:
							MissionList.back().DescrColor = sRGBCOLOR{eRGBCOLOR::white};
							break;
						}
					}
					MissionList.back().Descr = TMission.Content;
				} else if (TMission.Name == "Icon") {
					MissionList.back().Icon = TMission.Content;
				} else if (TMission.Name == "File") {
					MissionList.back().File = TMission.Content;
				}
			}
		}
	}

	AllMission = MissionList.size();
}












//------------------------------------------------------------------------------------
// выбор миссии
//------------------------------------------------------------------------------------
void MissionMenu()
{

	// проверка ограничения
	if (GameConfig().Profile[CurrentProfile].OpenLevelNum > AllMission-1)
		ChangeGameConfig().Profile[CurrentProfile].OpenLevelNum = AllMission-1;



	sRECT SrcRect, DstRect;
	SrcRect(2, 2, 863-2, 484-2);
	DstRect(GameConfig().InternalWidth/2-427, 175-15, GameConfig().InternalWidth/2-427+863-4, 175-15+484-4);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/panel800_444_back.tga"), true, 0.9f * MenuContentTransp);



	int X1 = GameConfig().InternalWidth/2 - 372;
	int Y1 = 270;




	// выводим текущий профиль пилота
	int Size = vw_TextWidth("%s: ", vw_GetText("Pilot Profile"));
	vw_DrawText(X1, 208+12, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, MenuContentTransp, "%s: ", vw_GetText("Pilot Profile"));

	if ((Size + vw_TextWidth(GameConfig().Profile[CurrentProfile].Name)) > 500) {
		vw_DrawText(X1+Size, 208+12, 0, 500-Size, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, GameConfig().Profile[CurrentProfile].Name);
		vw_DrawText(X1+510, 208+12, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, "...");
	} else
		vw_DrawText(X1+Size, 208+12, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, GameConfig().Profile[CurrentProfile].Name);

	if (DrawButton200_2(X1+616-72, 212, vw_GetText("Change Profile"), MenuContentTransp, false)) {
		cCommand::GetInstance().Set(eCommand::SWITCH_TO_PROFILE);
	}




	// подложка для вывода описания миссий
	SrcRect(0,0,2,2);
	DstRect(X1-2,Y1-2,X1+2+710,Y1+2+320);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.2f*MenuContentTransp);
	DstRect(X1,Y1,X1+710,Y1+320);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.5f*MenuContentTransp);




	// подсвечиваем выбранный уровень
	if (CurrentMission != -1)
		if (StartMission<=CurrentMission && CurrentMission<=EndMission) {
			int ShowLine = CurrentMission;
			if (ShowLine>=StartMission) ShowLine -= StartMission;

			SrcRect(0,0,2,2);
			DstRect(X1+1,Y1 + 64*ShowLine+1,X1+709,Y1 + 64*ShowLine+63);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/whitepoint.tga"), true, 0.1f*MenuContentTransp);
		}




	// выводим миссии текущего листа
	int TMPSoundOnMissionID = -1;
	for (int i=StartMission; i<=EndMission; i++)
		if (AllMission > i) {
			// если не можем выбирать...
			if (i > GameConfig().Profile[CurrentProfile].OpenLevelNum) {
				SrcRect(0,0,64,64);
				DstRect(X1+2,Y1+2,X1+62,Y1+62);

				vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(MissionList[i].Icon), true, 0.3f*MenuContentTransp);
				vw_DrawText(X1+20+64, Y1+9, -610, 0, 1.0f, MissionList[i].TitleColor, 0.3f*MenuContentTransp, vw_GetText(MissionList[i].Title));
				vw_DrawText(X1+20+64, Y1+33, -610, 0, 1.0f, MissionList[i].DescrColor, 0.3f*MenuContentTransp, vw_GetText(MissionList[i].Descr));
			}


			DstRect(X1, Y1+1, X1+710, Y1+64);
			if (i <= GameConfig().Profile[CurrentProfile].OpenLevelNum) {
				// работаем с клавиатурой
				if ((MenuContentTransp >= 0.99f) && !isDialogBoxDrawing()) CurrentActiveMenuElement++;
				bool InFocusByKeyboard = false;
				if (CurrentKeyboardSelectMenuElement > 0) {
					if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement) {
						InFocusByKeyboard = true;
					}
				}


				if ((vw_MouseOverRect(DstRect) || InFocusByKeyboard) && !isDialogBoxDrawing()) {
					TMPSoundOnMissionID = i;
					SetCursorStatus(eCursorStatus::ActionAllowed);
					// если только встали - нужно звуком это показать
					if (SoundOnMissionID != i) {
						SoundOnMissionID = i;
						// если задействуем клавиатуру - неиграем тут звук
						if (CurrentKeyboardSelectMenuElement == 0)
							PlayMenuSFX(eMenuSFX::OverLine, 1.0f);
					}

					// если стоим над ним...
					SrcRect(0,0,64,64);
					DstRect(X1,Y1,X1+64,Y1+64);
					vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(MissionList[i].Icon), true, MenuContentTransp);
					vw_DrawText(X1+20+64, Y1+9, -610, 0, 1.0f, MissionList[i].TitleColor, MenuContentTransp, vw_GetText(MissionList[i].Title));
					vw_DrawText(X1+20+64, Y1+33, -610, 0, 1.0f, MissionList[i].DescrColor, MenuContentTransp, vw_GetText(MissionList[i].Descr));


					if (CurrentMission != i) {
						SrcRect(0,0,2,2);
						DstRect(X1+64,Y1+1,X1+709,Y1+63);
						vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/whitepoint.tga"), true, 0.1f*MenuContentTransp);
					}
					if (vw_GetMouseLeftClick(true) || (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN)))) {

						CurrentMission = i;
						ChangeGameConfig().Profile[CurrentProfile].LastMission = CurrentMission;
						PlayMenuSFX(eMenuSFX::SelectLine, 1.0f);
						if (InFocusByKeyboard) {
							vw_SetKeyStatus(SDLK_KP_ENTER, false);
							vw_SetKeyStatus(SDLK_RETURN, false);
						}
					}

					if (vw_GetMouseLeftDoubleClick(true)) {
						CurrentMission = i;
						ChangeGameConfig().Profile[CurrentProfile].LastMission = CurrentMission;
						// если уже играли в эту миссию
						if (GameConfig().Profile[CurrentProfile].MissionReplayCount[CurrentMission] > 0) {
							if (GameConfig().NeedShowHint[5]) {
								SetCurrentDialogBox(eDialogBox::StartMissionSecondTime);
							} else {
								cCommand::GetInstance().Set(eCommand::SWITCH_TO_WORKSHOP);
								CurrentWorkshop = 3;
								WorkshopCreate();
							}
						} else {
							cCommand::GetInstance().Set(eCommand::SWITCH_TO_WORKSHOP);
							CurrentWorkshop = 3;
							WorkshopCreate();
						}
					}
				} else {
					// если не стоим над ним, но можем выбирать
					SrcRect(0,0,64,64);
					DstRect(X1+2,Y1+2,X1+62,Y1+62);
					vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(MissionList[i].Icon), true, 0.8f*MenuContentTransp);
					vw_DrawText(X1+20+64, Y1+9, -610, 0, 1.0f, MissionList[i].TitleColor, 0.8f*MenuContentTransp, vw_GetText(MissionList[i].Title));
					vw_DrawText(X1+20+64, Y1+33, -610, 0, 1.0f, MissionList[i].DescrColor, 0.8f*MenuContentTransp, vw_GetText(MissionList[i].Descr));
				}
			}

			Y1 += 64;
		}
	// если не стоим над профилями - нужно сбросить флаг
	if (TMPSoundOnMissionID == -1)
		SoundOnMissionID = -1;





	Y1 = 270;
	// стрелки перемещения списка
	if (DrawListUpButton(X1+718, Y1, MenuContentTransp, !(StartMission > 0))) {
		StartMission--;
		EndMission--;
	}

	if (DrawListDownButton(X1+718,Y1+320-32, MenuContentTransp, !(StartMission < AllMission-5))) {
		StartMission++;
		EndMission++;
	}
	// проверяем колесико мышки, если курсор находится над активной частью
	DstRect(X1,Y1,X1+750,Y1+320);
	if (vw_MouseOverRect(DstRect)) {
		if (vw_GetWheelStatus() != 0 && !isDialogBoxDrawing()) {
			StartMission += vw_GetWheelStatus();
			EndMission += vw_GetWheelStatus();

			if (StartMission < 0) {
				StartMission = 0;
				EndMission = 4;
			}
			if (EndMission > AllMission-1) {
				EndMission = AllMission-1;
				StartMission = EndMission-4;
			}

			vw_ResetWheelStatus();
		}
	} else if (vw_GetWheelStatus() != 0) {
		vw_ResetWheelStatus();
	}

	// выводим отображение положени в списке на полоске со стрелками
	SrcRect(0,0,32,32);
	DstRect(X1+750-32+4,Y1+32+((320.0f-64)/AllMission)*StartMission,X1+750-4,Y1+32+((320.0f-64)/AllMission)*(EndMission+1));
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/whitepoint.tga"), true, 0.3f*MenuContentTransp);

	// обработка перетягивания ползунка отображения позиции списка
	// если стоим на ползунком и нажали кнопку мышки - "захватываем"
	if (!SliderUnderMouseControl && vw_MouseOverRect(DstRect) && vw_GetMouseLeftClick(false) && !isDialogBoxDrawing()) {
		SliderUnderMouseControl = true;
		PlayMenuSFX(eMenuSFX::Click, 1.0f);
	}
	// если ползунок был захвачен, но уже не над секцией где его можно перетягивать или отпустили мышку - отпускаем
	sRECT DstRect2;
	DstRect2(X1+750-32+4,Y1+32,X1+750-4,Y1+32+(320.0f-64));
	if ((SliderUnderMouseControl && (!vw_MouseOverRect(DstRect2) || !vw_GetMouseLeftClick(false))) || isDialogBoxDrawing()) {
		SliderUnderMouseControl = false;
	}
	// просто кликнули на зону перетягивания, не на ползунок
	if (!vw_MouseOverRect(DstRect) && vw_MouseOverRect(DstRect2) && vw_GetMouseLeftClick(false) && !isDialogBoxDrawing()) {
		SliderUnderMouseControl = true;
		PlayMenuSFX(eMenuSFX::Click, 1.0f);
		vw_SetMouseLeftClick(false);
	}
	// отображаем курсором, что можно кликать на полосе прокрутки
	if (vw_MouseOverRect(DstRect2))
		SetCursorStatus(eCursorStatus::ActionAllowed);
	// корректируем его положение ползунка согласно положению мышки
	if (SliderUnderMouseControl) {
		int MouseX, MouseY;
		vw_GetMousePos(MouseX, MouseY);
		int SliderNewPosition = (MouseY - Y1-32)/((320.0f-64)/AllMission);

		StartMission = 0;
		EndMission = 4;
		if (SliderNewPosition > 2) {
			StartMission = SliderNewPosition-2;
			EndMission = SliderNewPosition+2;

			if (SliderNewPosition >= AllMission-2) {
				StartMission = AllMission-5;
				EndMission = AllMission-1;
			}
		}
	}






	int X = GameConfig().InternalWidth / 2 - 284;
	int Y = 165 + 100 * 5;
	if (DrawButton256(X,Y, vw_GetText("MAIN MENU"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime))
		cCommand::GetInstance().Set(eCommand::SWITCH_TO_MAIN_MENU);

	X = GameConfig().InternalWidth / 2 + 28;
	if (DrawButton256(X,Y, vw_GetText("NEXT"), MenuContentTransp, &Button11Transp, &LastButton11UpdateTime, !(CurrentMission >= 0))) {
		// если уже играли в эту миссию
		if (GameConfig().Profile[CurrentProfile].MissionReplayCount[CurrentMission] > 0) {
			if (GameConfig().NeedShowHint[5]) {
				SetCurrentDialogBox(eDialogBox::StartMissionSecondTime);
			} else {
				cCommand::GetInstance().Set(eCommand::SWITCH_TO_WORKSHOP);
				CurrentWorkshop = 3;
				WorkshopCreate();
			}
		} else {
			cCommand::GetInstance().Set(eCommand::SWITCH_TO_WORKSHOP);
			CurrentWorkshop = 3;
			WorkshopCreate();
		}
	}




}

} // astromenace namespace
} // viewizard namespace
