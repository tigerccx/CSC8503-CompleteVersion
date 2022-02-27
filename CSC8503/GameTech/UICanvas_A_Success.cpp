#include "UICanvas_A_Success.h"
#include "SharedMacros.h"
#include <string>

void UICanvas_A_Success::Init()
{
	char buf[128];
	snprintf(buf, 128, "It took you: %.2fs", timeCount);
	AddUIElement("TimeDisplay", new UIElementDebugWord(buf, Vector2(20, 35), UI_ELEMENT_DEFAULT_COLOUR));
	AddUIElement("ReturnToMainMenu", new UIElementDebugWord("Press (E) to return to menu.", Vector2(20, 40), UI_ELEMENT_DEFAULT_COLOUR));
}

