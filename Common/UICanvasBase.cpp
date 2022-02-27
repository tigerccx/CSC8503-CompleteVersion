#include "UICanvasBase.h"


bool UICanvasBase::AddUIElement(const std::string& name, UIElementBase* element)
{
	auto itr = elements.find(name);
	if (itr != elements.end()) {
		return false;
	}
	elements[name] = element;
	return true;
}

UIElementBase* UICanvasBase::GetUIElement(const std::string& name)
{
	auto itr = elements.find(name);
	if (itr != elements.end()) {
		return itr->second;
	}
	else {
		return nullptr;
	}
}

bool UICanvasBase::RemoveUIElement(const std::string& name)
{
	auto itr = elements.find(name);
	if (itr != elements.end()) {
		if(itr->second)
			delete itr->second;
		elements.erase(itr);
		return true;
	}
	else
	{
		return false;
	}
}

void UICanvasBase::ClearUIElements()
{
	auto itr = elements.begin();
	while (elements.size() != 0) {
		if (itr->second)
			delete itr->second;
		itr = elements.erase(itr);
	}
}
