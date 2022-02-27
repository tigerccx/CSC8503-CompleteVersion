#include "EnemyFollow.h"
#include "TutorialGame.h"
#include "../CSC8503Common/GameObject.h"
#include "GameWorld_B.h"
#include "NavigationController.h"
#include "../CSC8503Common/NavigationPath.h"
#include "../CSC8503Common/NavigationMesh.h"
#include "WorldBController.h"

float EnemyFollow::posY = 0;

void EnemyFollow::Update(float dt)
{
	Vector3 posCur = GetGO()->GetTransform().GetPosition();
	Vector3 posDst = goFollow->GetTransform().GetPosition();

	if ((posCur - posDst).Length() < 0.4f) {
		((GameWorld_B*)TutorialGame::GetInstance()->world)->gameController->ResetScene();
		return;
	}

	timeCur += dt;
	if (timeCur > pathFindInterval) { //Update nav
		timeCur = 0;
		ResetNavigationState();

		auto navContoller = ((GameWorld_B*)TutorialGame::GetInstance()->world)->navController;
		NavMeshPathFindState state = navContoller->FindPath(posCur, posDst, navPath);
		if (state== NavMeshPathFindState_UnableToFindPath) {
			//If can't find way, do something?
			std::cout << "Unable to find path!" << std::endl;
			curFrom = curTo = posCur;
		}
		else if (state == NavMeshPathFindState_ToNotOnGrid)
		{
			std::cout << "Followed object off mesh!" << std::endl;
			curFrom = curTo = posCur;
		}
		else if (state == NavMeshPathFindState_FromNotOnGrid)
		{
			curFrom = curTo = navContoller->GetNearestVert(posCur);
			curFrom.y = posY;
			curTo.y = posY;
			GetGO()->GetTransform().SetPosition(curFrom);
		}
		else
		{
			if (navPath.GetSize() >= 2) {
				curDis = 0;
				navPath.PopWaypoint(curFrom);
				navPath.PopWaypoint(curTo);
			}
			else
			{
				navPath.Clear();
				curFrom = curTo = posCur;
			}
		}
	}

	if (navPath.GetSize() >= 2) {
		for (int i = 0; i < navPath.GetSize() - 1; ++i) {
			Debug::DrawLine(navPath.waypoints[i], navPath.waypoints[i + 1]);
		}
	}

	curFrom.y = posY;
	curTo.y = posY;

	if (Maths::ApproxEqual(curFrom, curTo) && navPath.GetSize() == 0) {
		return;
	}
	curDis += speedFollow * dt;

	while (curDis >= (curTo - curFrom).Length()) {
		curDis = curDis - (curTo - curFrom).Length();
		curFrom = curTo;
		if (navPath.GetSize() > 0) {
			navPath.PopWaypoint(curTo);
		}
		else
		{
			break;
		}
	}
	auto& transform = GetGO()->GetTransform();
	Vector3 posTest = transform.GetPosition();
	Vector3 newPos = (curTo - curFrom).Normalised() * curDis + curFrom;
	if ((newPos - posTest).Length() > speedFollow) {
		assert(false);
	}
	transform.SetPosition(newPos);
}

void EnemyFollow::ImportFromJson(Json::Value json)
{
	speedFollow = json["speedScale"].asDouble();
}

void EnemyFollow::ResetNavigationState()
{
	navPath.Clear();
	curDis = 0;
}


