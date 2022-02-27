#include "UIRendererDebugWord.h"
#include "../CSC8503Common/Debug.h"

UIRendererDebugWord::UIRendererDebugWord(GameTechRenderer* renderer)
{
	NCL::Debug::SetRenderer(renderer);
}
