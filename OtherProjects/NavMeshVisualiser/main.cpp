#include "../../Common/Window.h"
#include "../../CSC8503/CSC8503Common/NavigationMesh.h"
#include "NavMeshRenderer.h"
#include <string>
#include "../../CSC8503/CSC8503Common/Debug.h"
#include "../../CSC8503/CSC8503Common/CollisionDetection.h"

using namespace NCL;
using namespace CSC8503;
using namespace std;

void TestNavMesh(NavigationMesh* navMesh, NavigationPath& path) {
	
}

int main() {
	Window* w = Window::CreateGameWindow("NavMesh Tester", 1120, 768);
	w->SetConsolePosition(100, 0);
	if (!w->HasInitialised()) {
		return -1;
	}

	string filename = "smalltest.navmesh";
	NavMeshRenderer* renderer = new NavMeshRenderer(filename);
	NavigationMesh* navMesh = new NavigationMesh(filename);
	Debug::SetRenderer(renderer);

	Vector3 start = Vector3(50, 0, 20);
	Vector3 end = Vector3(100, 0, 20);

	w->LockMouseToWindow(true);
	w->ShowOSPointer(false);
	
	bool inSelectionMode = false;

	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
		float time = w->GetTimer()->GetTimeDeltaSeconds();

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
			inSelectionMode = !inSelectionMode;
			if (inSelectionMode) {
				Window::GetWindow()->ShowOSPointer(true);
				Window::GetWindow()->LockMouseToWindow(false);
			}
			else {
				Window::GetWindow()->ShowOSPointer(false);
				Window::GetWindow()->LockMouseToWindow(true);
			}
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
			Debug::DrawLine(Vector3(0, 0, 0), Vector3(10, 0, 0), Vector4(1, 0, 0, 1), 5.0f);
			Debug::DrawLine(Vector3(0, 0, 0), Vector3(0, 10, 0), Vector4(0, 1, 0, 1), 5.0f);
			Debug::DrawLine(Vector3(0, 0, 0), Vector3(0, 0, 10), Vector4(0, 0, 1, 1), 5.0f);
		}

		if (inSelectionMode) {
			int isSettingPoint = 0;
			if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::LEFT)) {
				isSettingPoint = 1;
			}
			if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
				isSettingPoint = 2;
			}
			if (isSettingPoint) {
				Ray ray = CollisionDetection::BuildRayFromMouse(*renderer->GetCamera());
				RayCollision collision;
				Plane p(Vector3(0, 1, 0), 0);//No height!
				if (CollisionDetection::RayPlaneIntersection(ray, p, collision)) {
					if (isSettingPoint == 1) {
						start = collision.collidedAt;
						Debug::DrawLine(start, start + Vector3(0, 100, 0), Vector4(0, 0, 1, 1), 5.0f);
					}
					else if (isSettingPoint == 2)
					{
						end = collision.collidedAt;
						Debug::DrawLine(end, end + Vector3(0, 100, 0), Vector4(1, 0, 1, 1), 5.0f);
					}
				}
			}
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
			Debug::DrawLine(start, start+Vector3(0, 100, 0), Vector4(0, 0, 1, 1), 5.0f);
			Debug::DrawLine(end, end + Vector3(0, 100, 0), Vector4(1, 0, 1, 1), 5.0f);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::R)) {
			navMesh->enableSmoothing = !navMesh->enableSmoothing;
			cout << "Pathway smoothing: " << navMesh->enableSmoothing << endl;
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F)) {
			NavigationPath pathway;
			navMesh->FindPath(start, end, pathway);

			Vector3 wayPointCur;
			Vector3 wayPointNext;
			pathway.PopWaypoint(wayPointCur);
			while (pathway.PopWaypoint(wayPointNext)) {
				Debug::DrawLine(wayPointCur, wayPointNext, Vector4(1, 1, 1, 1), 5.0f);
				wayPointCur = wayPointNext;
			}
			
		}


		if(!inSelectionMode)
			renderer->Update(time);

		Debug::FlushRenderables(time);
		renderer->Render();
	}

	delete renderer;

	Window::DestroyGameWindow();
}