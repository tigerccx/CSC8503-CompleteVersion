#pragma once
#include <string>
#include <map>

#include "UIElementBase.h"

class UIRendererBase;

class UICanvasBase
{
public:
	virtual ~UICanvasBase(){ ClearUIElements(); }

	virtual void Init() = 0;

	friend class UIRendererBase;
	bool AddUIElement(const std::string& name, UIElementBase* element);
	UIElementBase* GetUIElement(const std::string& name);
	bool RemoveUIElement(const std::string& name);
	void ClearUIElements();

protected:
	std::map<std::string, UIElementBase*> elements;
};

