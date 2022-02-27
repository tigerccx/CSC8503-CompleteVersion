#include "UIElementDebugWord.h"

void UIElementDebugWord::Render()
{
	Debug::Print(words, pos, colour, size);
}
