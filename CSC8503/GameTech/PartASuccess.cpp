#include "PartASuccess.h"
#include "../CSC8503Common/GameObject.h"
#include "MainBall.h"
#include "TutorialGame.h"
#include "GameWorld_A.h"
#include "WorldAController.h"

void PartASuccess::OnCollisionBegin(GameObject* otherObject)
{
	if(otherObject->GetComponent<MainBall>()) {
		((GameWorld_A*)TutorialGame::GetInstance()->world)->gameController->LoadSuccessScene();
	}
}
