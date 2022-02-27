#include "UICanvasMainMenu.h"
#include "SharedMacros.h"

void UICanvasMainMenu::Init()
{
	AddUIElement("Title",	 new UIElementDebugWord("Here's a Little Demo...", Vector2(20, 35), UI_ELEMENT_DEFAULT_COLOUR));
	AddUIElement("Option_1", new UIElementDebugWord("Press (1) to play Part A", Vector2(20, 40), UI_ELEMENT_DEFAULT_COLOUR));
	AddUIElement("Option_2", new UIElementDebugWord("Press (2) to play Part B", Vector2(20, 45), UI_ELEMENT_DEFAULT_COLOUR));
	AddUIElement("Option_3", new UIElementDebugWord("Press (Esc) to exit", Vector2(20, 50), UI_ELEMENT_DEFAULT_COLOUR));
}
