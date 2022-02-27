#pragma once
#include "../CSC8503Common/Component.h"
#include "../CSC8503Common/GameObject.h"
#include "MainBall.h"

using namespace NCL;
using namespace NCL::CSC8503;

class KillingPlane :
	public Component {
public:
	KillingPlane() = default;
	~KillingPlane() = default;

	void OnCollisionBegin(GameObject* otherObject) override;
protected:
	bool resetTrigger = false;
};