#include "ScoreRecorder.h"

void ScoreRecorder::Update(float dt)
{
	int decrease = int(decreaseSpeed * dt);
	score -= decrease;
	if (score < 0) {
		score = 0;
	}
}