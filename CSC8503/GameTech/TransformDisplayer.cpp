#include "TransformDisplayer.h"
#include "TutorialGame.h"
#include "../../Common/UICanvasBase.h"
#include "UIElementDebugWord.h"
#include "../CSC8503Common/GameObject.h"
#include "SharedMacros.h"

#define __UI_ELEMENT_TRANSFORM_DISPLAY_NAME_TITLE "__transform_display_title"
#define __UI_ELEMENT_TRANSFORM_DISPLAY_NAME_POS "__transform_display_pos"
#define __UI_ELEMENT_TRANSFORM_DISPLAY_NAME_ROT "__transform_display_rot"

TransformDisplayer::~TransformDisplayer()
{
	ClearElements();
}

void TransformDisplayer::Update(float dt)
{
	if (GetGO()) {
		auto elementTitle = (UIElementDebugWord*)TutorialGame::GetInstance()->uiCanvas->GetUIElement(__UI_ELEMENT_TRANSFORM_DISPLAY_NAME_TITLE);
		if (!elementTitle) {
			elementTitle = new UIElementDebugWord("", Vector2(UI_ELEMENT_DEBUGINFO_X_OFFSET, UI_ELEMENT_DEBUGINFO_Y_OFFSET+8), UI_ELEMENT_DEBUGINFO_COLOUR, 15);
			TutorialGame::GetInstance()->uiCanvas->AddUIElement(__UI_ELEMENT_TRANSFORM_DISPLAY_NAME_TITLE, elementTitle);
		}
		elementTitle->words = "GO: " + GetGO()->GetName();


		char buf[128];
		auto transform = GetGO()->GetTransform();

		auto elementPos = (UIElementDebugWord*)TutorialGame::GetInstance()->uiCanvas->GetUIElement(__UI_ELEMENT_TRANSFORM_DISPLAY_NAME_POS);
		if (!elementPos) {
			elementPos = new UIElementDebugWord("", Vector2(UI_ELEMENT_DEBUGINFO_X_OFFSET, UI_ELEMENT_DEBUGINFO_Y_OFFSET+10), UI_ELEMENT_DEBUGINFO_COLOUR, 15);
			TutorialGame::GetInstance()->uiCanvas->AddUIElement(__UI_ELEMENT_TRANSFORM_DISPLAY_NAME_POS, elementPos);
		}

		Vector3 pos = transform.GetPosition();
		snprintf(buf, 128, "POS: %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);
		elementPos->words = buf;

		auto elementRot = (UIElementDebugWord*)TutorialGame::GetInstance()->uiCanvas->GetUIElement(__UI_ELEMENT_TRANSFORM_DISPLAY_NAME_ROT);
		if (!elementRot) {
			elementRot = new UIElementDebugWord("", Vector2(UI_ELEMENT_DEBUGINFO_X_OFFSET, UI_ELEMENT_DEBUGINFO_Y_OFFSET+12), UI_ELEMENT_DEBUGINFO_COLOUR, 15);
			TutorialGame::GetInstance()->uiCanvas->AddUIElement(__UI_ELEMENT_TRANSFORM_DISPLAY_NAME_ROT, elementRot);
		}
		Vector3 rot = transform.GetOrientation().ToEuler();
		snprintf(buf, 128, "ROT: %.2f, %.2f, %.2f", rot.x, rot.y, rot.z);
		elementRot->words = buf;
	}
	else
	{
		ClearElements();
	}
}

void TransformDisplayer::ClearElements()
{
	TutorialGame::GetInstance()->uiCanvas->RemoveUIElement(__UI_ELEMENT_TRANSFORM_DISPLAY_NAME_TITLE);
	TutorialGame::GetInstance()->uiCanvas->RemoveUIElement(__UI_ELEMENT_TRANSFORM_DISPLAY_NAME_POS);
	TutorialGame::GetInstance()->uiCanvas->RemoveUIElement(__UI_ELEMENT_TRANSFORM_DISPLAY_NAME_ROT);
}
