#include "UICanvas_B.h"
#include "SharedMacros.h"

void UICanvas_B::Init()
{
	AddUIElement("Tip_0", new UIElementDebugWord("This is demo B.", Vector2(UI_ELEMENT_BOTTOM_X_OFFSET, UI_ELEMENT_BOTTOM_Y_OFFSET), UI_ELEMENT_DEFAULT_COLOUR));
	AddUIElement("Tip_1", new UIElementDebugWord("Try to collect all the balls. (R) to reset.", Vector2(UI_ELEMENT_BOTTOM_X_OFFSET, UI_ELEMENT_BOTTOM_Y_OFFSET+2), UI_ELEMENT_DEFAULT_COLOUR, 15.0f));
}
