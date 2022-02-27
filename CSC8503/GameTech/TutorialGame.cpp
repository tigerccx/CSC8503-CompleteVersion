#include "TutorialGame.h"

#include "../CSC8503Common/PhysicsSystem.h"

#include "../CSC8503Common/GameWorld.h"
#include "../CSC8503Common/WorldHierarchy.h"


#include "../CSC8503Common/PushdownState.h"
#include "../CSC8503Common/PushdownMachine.h"

#include "../CSC8503Common/PositionConstraint.h"
#include "../CSC8503Common/PositionConstraint1.h"
#include "../CSC8503Common/RotationConstraint.h"
#include "../CSC8503Common/LinearMovementConstraint.h"

#include "GameTechAssetManager.h"
#include "SceneCreator.h"

#include "GameTechRenderer.h"
#include "UIRendererDebugWord.h"

#include "GameWorld_Menu.h"
#include "UICanvasMainMenu.h"

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame()	{
	renderer = new GameTechRenderer();
	
	uiRenderer = new UIRendererDebugWord(renderer);
	
	Layer::Init();
	physics = new PhysicsSystem();
	physics->SetGravity(Vector3(0, -9.8, 0));
	physics->UseGravity(true);
	physics->SetLinearDamping(0.4);
	forceMagnitude = 10.0f;

	assetManager = new GameTechAssetManager();
	assetManager->ReloadAllAssets();
	SceneCreator::instance.SetAssetManager(assetManager);


	SetWorld(new GameWorld_Menu());
	SetCanvas(new UICanvasMainMenu());
}

void NCL::CSC8503::TutorialGame::__DelayedSetWorld()
{
	if (__tempWorld) {
		if (world) {
			physics->RemoveAllCollisionsInCurrentWorld();
			delete world;
		}
		world = __tempWorld;
		world->InitWorld();
		renderer->SetWorld(world);
		physics->SetWorld(world);

		__tempWorld = nullptr;
	}	
}

void NCL::CSC8503::TutorialGame::__DelayedSetCanvas()
{
	if (__tempCanvas) {
		if (uiCanvas) {
			delete uiCanvas;
		}
		uiCanvas = __tempCanvas;
		uiCanvas->Init();
		uiRenderer->SetCanvas(uiCanvas);

		__tempCanvas = nullptr;
	}
}




//void NCL::CSC8503::TutorialGame::ResetPartB()
//{
//	InitCamera();
//	useGravity = true;
//	physics->UseGravity(useGravity);
//	inSelectionMode = false;
//	selectionObject = nullptr;
//
//	world->Clear();
//	physics->Clear();
//
//	std::string path = "../../Scenes/partB.json";
//	std::string pathLayers = "../../Scenes/partBLayer.json";
//	assert(InitLayerFromJson(pathLayers));
//	assert(LoadSceneFromJson(path));
//
//	vector<Vector2> pathPoints = {
//		Vector2(66.15, 23.8),
//		Vector2(65.67, 53.61),
//		Vector2(37.5, 54.1),
//		Vector2(38.2, 23.9)
//	};
//	AddEnemy0ToWorld(pathPoints);
//}


TutorialGame::~TutorialGame()	{
	delete physics;
	delete renderer;
	delete world;
}

TutorialGame* NCL::CSC8503::TutorialGame::GetInstance()
{
	static TutorialGame instance;
	return &instance;
}

bool TutorialGame::UpdateGame(float dt) {

	UpdateKeys();

	if (world) {
		world->UpdateHierarchy();

		physics->Update(dt);
		//SelectObject();

		//if (lockedObject != nullptr) {
		//	Vector3 objPos = lockedObject->GetTransform().GetPosition();
		//	Vector3 camPos = objPos + lockedOffset;

		//	Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		//	Matrix4 modelMat = temp.Inverse();

		//	Quaternion q(modelMat);
		//	Vector3 angles = q.ToEuler(); //nearly there now!

		//	world->GetMainCamera()->SetPosition(camPos);
		//	world->GetMainCamera()->SetPitch(angles.x);
		//	world->GetMainCamera()->SetYaw(angles.y);
		//}

		/*if (!inSelectionMode) {
			world->GetMainCamera()->UpdateCamera(dt);
		}*/

		world->UpdateWorld(dt);
		renderer->Update(dt);

		Debug::FlushRenderables(dt);
		renderer->Render();
	}
	
	if (uiCanvas) {
		uiRenderer->Update(dt);
		uiRenderer->Render();
	}

	return true;
}

void NCL::CSC8503::TutorialGame::SetWorld(GameWorld* world)
{
	__tempWorld = world;
}

void NCL::CSC8503::TutorialGame::SetCanvas(UICanvasBase* canvas)
{
	__tempCanvas = canvas;
}

void NCL::CSC8503::TutorialGame::PreUpdate()
{
	__DelayedSetCanvas();
	__DelayedSetWorld();
}

void TutorialGame::UpdateKeys() {

//#ifdef _DEBUG
//	//TODO: Too many draw lines make OpenGL complain... Ask Rich for clues.
//
//	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::V)) {
//		cout << "Dynamic Tree: " << endl;
//		SingletonWorldHierarchy::hierarchy.treeDynamic->DebugPrint();
//		cout << "Static Tree: " << endl;
//		SingletonWorldHierarchy::hierarchy.treeStatic->DebugPrint();
//	}
//	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::N)) {
//		SingletonWorldHierarchy::hierarchy.treeDynamic->DebugDraw(Vector4(0, 0, 1, 1));
//	}
//	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::M)) {
//		SingletonWorldHierarchy::hierarchy.treeStatic->DebugDraw(Vector4(1, 0, 0, 1));
//	}
//#endif


	//if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
	//	InitWorld(); //We can reset the simulation at any time with F1
	//	selectionObject = nullptr;
	//	lockedObject	= nullptr;
	//}

	//if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
	//	InitCamera(); //F2 will reset the camera to a specific default place
	//}

	//if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
	//	useGravity = !useGravity; //Toggle gravity!
	//	physics->UseGravity(useGravity);
	//}
	////Running certain physics updates in a consistent order might cause some
	////bias in the calculations - the same objects might keep 'winning' the constraintPosition
	////allowing the other one to stretch too much etc. Shuffling the order so that it
	////is random every frame can help reduce such bias.
	//if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
	//	world->ShuffleConstraints(true);
	//}
	//if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
	//	world->ShuffleConstraints(false);
	//}

	//if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
	//	world->ShuffleObjects(true);
	//}
	//if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
	//	world->ShuffleObjects(false);
	//}
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view		= world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld	= view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();

	Vector3 charForward  = lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);
	Vector3 charForward2 = lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);

	float force = 100.0f;

	if (lockedObject->GetPhysicsObject()) {
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			lockedObject->GetPhysicsObject()->AddForce(-rightAxis * force);
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			Vector3 worldPos = selectionObject->GetTransform().GetPosition();
			lockedObject->GetPhysicsObject()->AddForce(rightAxis * force);
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			lockedObject->GetPhysicsObject()->AddForce(fwdAxis * force);
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			lockedObject->GetPhysicsObject()->AddForce(-fwdAxis * force);
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NEXT)) {
			lockedObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}
}

void TutorialGame::DebugObjectMovement() {
//If we've selected an object, we can manipulate it with some key presses
	//if (inSelectionMode && selectionObject) {
	//	if (selectionObject->GetPhysicsObject()) {
	//		//Twist the selected object!
	//		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
	//			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
	//		}

	//		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
	//			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
	//		}

	//		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
	//			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
	//		}

	//		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
	//			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
	//		}

	//		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
	//			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
	//		}

	//		

	//		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
	//			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
	//		}
	//	}
	//}

}

/*

Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
bool TutorialGame::SelectObject() {
	//if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
	//	inSelectionMode = !inSelectionMode;
	//	if (inSelectionMode) {
	//		Window::GetWindow()->ShowOSPointer(true);
	//		Window::GetWindow()->LockMouseToWindow(false);
	//	}
	//	else {
	//		Window::GetWindow()->ShowOSPointer(false);
	//		Window::GetWindow()->LockMouseToWindow(true);
	//	}
	//}
	//if (inSelectionMode) {
	//	renderer->DrawString("Press Q to change to camera mode!", Vector2(5, 85));

	//	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::LEFT)) {
	//		if (selectionObject) {	//set colour to deselected;
	//			selectionObject->GetRenderObject()->SetColour(selectionObject->tint);
	//			selectionObject->OnUnSelected();
	//			selectionObject = nullptr;
	//			lockedObject	= nullptr;
	//		}

	//		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

	//		RayCollision closestCollision;
	//		if (CollisionDetection::Raycast(ray, closestCollision, world, true)) {
	//			selectionObject = (GameObject*)closestCollision.node;
	//			selectionObject->OnSelected();
	//			selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
	//			Debug::DrawLine(ray.GetPosition(), ray.GetPosition() + ray.GetDirection() * closestCollision.rayDistance, Vector4(1, 0, 0, 1), 5.0f);
	//			return true;
	//		}
	//		else {
	//			return false;
	//		}
	//	}
	//}
	//else {
	//	renderer->DrawString("Press (Q) to change to select mode!", Vector2(5, 80));
	//}

	//return false;
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/
void TutorialGame::MoveSelectedObject() {
	renderer->DrawString("Click Force:" + std::to_string(forceMagnitude), Vector2(10, 20));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;
	}

	//Push the selected!!!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
		RayCollision closestCollision;
		if (CollisionDetection::Raycast(ray, closestCollision, world, true)) {
			if (closestCollision.node == selectionObject) {
				auto phys = selectionObject->GetPhysicsObject();
				if(phys)
					phys->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}
}