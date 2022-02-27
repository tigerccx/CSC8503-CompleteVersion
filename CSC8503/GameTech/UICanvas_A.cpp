#include "UICanvas_A.h"
#include "SharedMacros.h"

void UICanvas_A::Init()
{
	AddUIElement("Tip_0", new UIElementDebugWord("This is demo A.", Vector2(UI_ELEMENT_BOTTOM_X_OFFSET, UI_ELEMENT_BOTTOM_Y_OFFSET), UI_ELEMENT_DEFAULT_COLOUR));
	AddUIElement("Tip_1", new UIElementDebugWord("Try to get the ball to the finish line. (R) to reset. ", Vector2(UI_ELEMENT_BOTTOM_X_OFFSET, UI_ELEMENT_BOTTOM_Y_OFFSET+2), UI_ELEMENT_DEFAULT_COLOUR, 15.0f));
	AddUIElement("Tip_Cam0", new UIElementDebugWord("Use (W, A, S, D) to move camera around, ", Vector2(UI_ELEMENT_BOTTOM_X_OFFSET, UI_ELEMENT_BOTTOM_Y_OFFSET+4), UI_ELEMENT_TIP_COLOUR, 15.0f));
	AddUIElement("Tip_Cam1", new UIElementDebugWord("(Q, E) to raise and lower camera.", Vector2(UI_ELEMENT_BOTTOM_X_OFFSET, UI_ELEMENT_BOTTOM_Y_OFFSET+6), UI_ELEMENT_TIP_COLOUR, 15.0f));
	AddUIElement("Tip_Cam2", new UIElementDebugWord("Hold (Mouse Right) to change view.", Vector2(UI_ELEMENT_BOTTOM_X_OFFSET, UI_ELEMENT_BOTTOM_Y_OFFSET+8), UI_ELEMENT_TIP_COLOUR, 15.0f));
}
