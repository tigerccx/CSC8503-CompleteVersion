#pragma once
#include "UIElementBase.h"
#include <map>
#include "UICanvasBase.h"

class UIRendererBase {
public:
	void Update(float dt);
	virtual void Render();

	void SetCanvas(UICanvasBase* canvas) { this->canvas = canvas; }
	UICanvasBase* GetCanvas() const { return canvas; }

protected:
	UICanvasBase* canvas;
};