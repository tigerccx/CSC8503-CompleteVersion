#include "CameraTransformDisplayer.h"
#include "TutorialGame.h"
#include "../../Common/UICanvasBase.h"
#include "UIElementDebugWord.h"
#include "../CSC8503Common/GameObject.h"
#include "SharedMacros.h"

#define __UI_ELEMENT_CAMTRANSFORM_DISPLAY_NAME_TITLE "__camtransform_display_title"
#define __UI_ELEMENT_CAMTRANSFORM_DISPLAY_NAME_POS "__camtransform_display_pos"
#define __UI_ELEMENT_CAMTRANSFORM_DISPLAY_NAME_ROT "__camtransform_display_rot"

CameraTransformDisplayer::~CameraTransformDisplayer()
{
	ClearElements();
}

void CameraTransformDisplayer::Update(float dt)
{
	if (camera) {
		auto elementTitle = (UIElementDebugWord*)TutorialGame::GetInstance()->uiCanvas->GetUIElement(__UI_ELEMENT_CAMTRANSFORM_DISPLAY_NAME_TITLE);
		if (!elementTitle) {
			elementTitle = new UIElementDebugWord("", Vector2(UI_ELEMENT_DEBUGINFO_X_OFFSET, UI_ELEMENT_DEBUGINFO_Y_OFFSET+2), UI_ELEMENT_DEBUGINFO_COLOUR, 15);
			TutorialGame::GetInstance()->uiCanvas->AddUIElement(__UI_ELEMENT_CAMTRANSFORM_DISPLAY_NAME_TITLE, elementTitle);
		}
		elementTitle->words = "Camera";


		char buf[128];

		auto elementPos = (UIElementDebugWord*)TutorialGame::GetInstance()->uiCanvas->GetUIElement(__UI_ELEMENT_CAMTRANSFORM_DISPLAY_NAME_POS);
		if (!elementPos) {
			elementPos = new UIElementDebugWord("", Vector2(UI_ELEMENT_DEBUGINFO_X_OFFSET, UI_ELEMENT_DEBUGINFO_Y_OFFSET+4), UI_ELEMENT_DEBUGINFO_COLOUR, 15);
			TutorialGame::GetInstance()->uiCanvas->AddUIElement(__UI_ELEMENT_CAMTRANSFORM_DISPLAY_NAME_POS, elementPos);
		}

		Vector3 pos = camera->position;
		snprintf(buf, 128, "POS: %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);
		elementPos->words = buf;

		auto elementRot = (UIElementDebugWord*)TutorialGame::GetInstance()->uiCanvas->GetUIElement(__UI_ELEMENT_CAMTRANSFORM_DISPLAY_NAME_ROT);
		if (!elementRot) {
			elementRot = new UIElementDebugWord("", Vector2(UI_ELEMENT_DEBUGINFO_X_OFFSET, UI_ELEMENT_DEBUGINFO_Y_OFFSET+6), UI_ELEMENT_DEBUGINFO_COLOUR, 15);
			TutorialGame::GetInstance()->uiCanvas->AddUIElement(__UI_ELEMENT_CAMTRANSFORM_DISPLAY_NAME_ROT, elementRot);
		}
		Vector3 rot = Vector3(camera->pitch, camera->yaw, 0);
		snprintf(buf, 128, "ROT: %.2f, %.2f, %.2f", rot.x, rot.y, rot.z);
		elementRot->words = buf;
	}
}

void CameraTransformDisplayer::ClearElements()
{
	TutorialGame::GetInstance()->uiCanvas->RemoveUIElement(__UI_ELEMENT_CAMTRANSFORM_DISPLAY_NAME_TITLE);
	TutorialGame::GetInstance()->uiCanvas->RemoveUIElement(__UI_ELEMENT_CAMTRANSFORM_DISPLAY_NAME_POS);
	TutorialGame::GetInstance()->uiCanvas->RemoveUIElement(__UI_ELEMENT_CAMTRANSFORM_DISPLAY_NAME_ROT);
}
