#include "KillingPlane.h"
#include "TutorialGame.h"
#include "GameWorld_A.h"
#include "WorldAController.h"

void KillingPlane::OnCollisionBegin(GameObject* otherObject) {
	auto ball = otherObject->GetComponent<MainBall>();
	if (ball) {
		((GameWorld_A*)TutorialGame::GetInstance()->world)->gameController->ResetScene();
	}
}