#include "UIRendererBase.h"

void UIRendererBase::Update(float dt)
{
	for (auto itr = canvas->elements.begin(); itr != canvas->elements.end();++itr) {
		itr->second->Update(dt);
		if (itr->second->IsExpired()) {
			itr = canvas->elements.erase(itr);
		}
	}
}

void UIRendererBase::Render()
{
	for (auto& itr : canvas->elements) {
		if (itr.second->isActive) {
			itr.second->Render();
		}
	}
}
