#pragma once
#include "../CSC8503Common/GameObject.h"
#include "../CSC8503Common/Component.h"

using namespace NCL;
using namespace NCL::CSC8503;

class ScoreRecorder :
	public Component {
public:
	ScoreRecorder() = default;
	~ScoreRecorder() = default;

	long int score = 100000;
	float decreaseSpeed = 100;

	virtual void Update(float dt) override;
};