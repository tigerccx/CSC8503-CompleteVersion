#include "../../Common/Window.h"

#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"

#include "../CSC8503Common/BehaviourNode.h"

#include "../CSC8503Common/PushdownState.h"
#include "../CSC8503Common/PushdownMachine.h"

#include "../CSC8503Common/NavigationGrid.h"

#include "TutorialGame.h"

using namespace NCL;
using namespace CSC8503;


TutorialGame* g;

//void TestStateMachine() {
//	StateMachine* testMachine = new StateMachine();
//	int data = 0;
//	int thresA2B = 10;
//	int thresB2A = 5;
//
//	State* A = new State([&](float dt, void* v)->void {
//		std::cout << "I'm in state A!" << std::endl;
//		data++;
//		}, nullptr);
//	State* B = new State([&](float dt, void* v)->void {
//		std::cout << "I'm in state B!" << std::endl;
//		data--;
//		}, nullptr);
//
//	StateTransition* stateAB = new StateTransition(
//		StateTransition::GreaterThanTransition<int>, &data, &thresA2B, A, B);
//	StateTransition* stateBA = new StateTransition(
//		StateTransition::LessThanTransition<int>, &data, &thresB2A, B, A);
//
//	testMachine->AddState(A);
//	testMachine->AddState(B);
//	testMachine->AddTransition(stateAB);
//	testMachine->AddTransition(stateBA);
//
//	for (int i = 0; i < 100; ++i) {
//		testMachine->Update(1.0f);
//	}
//}
//
//vector<Vector3> testNodes;
//void TestPathfinding() {
//	NavigationGrid grid("TestGrid1.txt");
//
//	NavigationPath outPath;
//
//	Vector3 startPos(80, 0, 10);
//	Vector3 endPos(80, 0, 80);
//
//	vector<NavigationMapPathfindingRequest> reqs;
//
//	for (int i = 0; i < 5; ++i) {
//		reqs.emplace_back(NavigationMapPathfindingRequest(startPos, endPos));
//	}
//
//	grid.FindPathsMultiThread(reqs);
//
//	//bool found = grid.FindPath(startPos, endPos, outPath);
//
//	for (int i = 0; i < reqs.size(); ++i) {
//		cout << reqs[i].found << endl;
//	}
//
//	Vector3 pos;
//	while (reqs[0].path.PopWaypoint(pos)) {
//		testNodes.push_back(pos);
//	}
//}
//void DisplayPathfinding() {
//	for (int i = 1; i < testNodes.size(); ++i) {
//		Vector3 a = testNodes[i - 1];
//		Vector3 b = testNodes[i];
//		a.y = b.y = 5;
//		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1), 10.0f);
//	}
//}
//
//void TestBehaviourTree() {
//	float behaviourTimer;
//	float distanceToTarget;
//	BehaviourAction* findKey = new BehaviourAction(
//		"Find Key",
//		[&](float dt, BehaviourState state)->BehaviourState {
//			if (state == BehaviourState_Initialise) {
//				std::cout << "Looking for a key!\n";
//				behaviourTimer = rand() % 100;
//				state = BehaviourState_Ongoing;
//			}
//			else if (state == BehaviourState_Ongoing) {
//				behaviourTimer -= dt;
//				if (behaviourTimer <= 0.0f) {
//					std::cout << "Found a key!\n";
//					return BehaviourState_Success;
//				}
//			}
//			return state;
//		}
//	);
//	BehaviourAction* goToRoom = new BehaviourAction(
//		"Go To Room",
//		[&](float dt, BehaviourState state)->BehaviourState {
//			if (state == BehaviourState_Initialise) {
//				std::cout << "Going to the loot room!\n";
//				state = BehaviourState_Ongoing;
//			}
//			else if (state == BehaviourState_Ongoing) {
//				distanceToTarget -= dt;
//				if (distanceToTarget <= 0.0f) {
//					std::cout << "Reached room!\n";
//					return BehaviourState_Success;
//				}
//			}
//			return state;
//		}
//	);
//	BehaviourAction* openDoor = new BehaviourAction(
//		"Open Door",
//		[&](float dt, BehaviourState state)->BehaviourState {
//			if (state == BehaviourState_Initialise) {
//				std::cout << "Opening Door!\n";
//				return BehaviourState_Success;
//			}
//		return state;
//		}
//	);
//	BehaviourAction* lookForTreasure = new BehaviourAction(
//		"Look For Treasure",
//		[&](float dt, BehaviourState state)->BehaviourState {
//			if (state == BehaviourState_Initialise) {
//				std::cout << "Looking for treasure !\n";
//				return BehaviourState_Ongoing;
//			}
//			else if (state == BehaviourState_Ongoing) {
//				bool found = rand() % 2;
//				if (found) {
//					std::cout << "I found some treasure !\n";
//					return BehaviourState_Success;
//									}
//				std::cout << "No treasure in here ...\n";
//				return BehaviourState_Failure;
//			}
//			return state;
//		}
//	);
//	BehaviourAction* lookForItems = new BehaviourAction(
//		"Look For Items",
//		[&](float dt, BehaviourState state)->BehaviourState {
//			if (state == BehaviourState_Initialise) {
//				std::cout << "Looking for items!\n";
//				return BehaviourState_Ongoing;
//			}
//			else if (state == BehaviourState_Ongoing) {
//				bool found = rand() % 2;
//				if (found) {
//					std::cout << "I found some items!\n";
//					return BehaviourState_Success;
//					
//				}
//				std::cout << "No items in here ...\n";
//				return BehaviourState_Failure;
//			}
//			return state;
//		}
//	);
//	BehaviourAction* shootAWayOut = new BehaviourAction(
//		"Shoot a Way Out",
//		[&](float dt, BehaviourState state)->BehaviourState {
//			if (state == BehaviourState_Initialise) {
//				std::cout << "Surrounded! Shooting my way out!\n";
//				behaviourTimer = rand() % 100;
//				return BehaviourState_Ongoing;
//			}
//			else if (state == BehaviourState_Ongoing) {
//				behaviourTimer -= dt;
//				if (behaviourTimer < 0) {
//					std::cout << "All clear!\n";
//					return BehaviourState_Success;
//				}
//				bool outOfAmmo = rand() % 100 == 0;
//				if (outOfAmmo) {
//					std::cout << "I ran out of ammo ...\n";
//					return BehaviourState_Failure;
//				}
//			}
//			return state;
//		}
//	);
//	BehaviourAction* dodgeBullets = new BehaviourAction(
//		"Dodge Bullets",
//		[&](float dt, BehaviourState state)->BehaviourState {
//			if (state == BehaviourState_Initialise) {
//				std::cout << "Surrounded! Dodging bullets!\n";
//				behaviourTimer = rand() % 100;
//				return BehaviourState_Ongoing;
//			}
//			else if (state == BehaviourState_Ongoing) {
//				behaviourTimer -= dt;
//				if (behaviourTimer < 0) {
//					std::cout << "Nice move!\n";
//					return BehaviourState_Success;
//				}
//				bool hitByBullet = rand() % 100 == 0;
//				if (hitByBullet) {
//					std::cout << "Ah here we go again ...\n";
//					return BehaviourState_Failure;
//				}
//			}
//			return state;
//		}
//	);
//	BehaviourDecorator* invertDodgeBullets = new BehaviourDecorator(
//		"Invertor",
//		dodgeBullets,
//		BehaviourDecorator::Invertor,
//		nullptr
//	);
//
//
//	BehaviourSequence* sequence =
//		new BehaviourSequence("Room Sequence");
//	sequence->AddChild(findKey);
//	sequence->AddChild(goToRoom);
//	sequence->AddChild(openDoor);
//
//	BehaviourSelector* selection =
//		new BehaviourSelector("Loot Selection");
//	selection->AddChild(lookForTreasure);
//	selection->AddChild(lookForItems);
//
//	BehaviourParallel* parallel =
//		new BehaviourParallel("Escape");
//	parallel->AddChild(shootAWayOut);
//	parallel->AddChild(invertDodgeBullets);
//
//	BehaviourSequence* rootSequence =
//		new BehaviourSequence("Root Sequence");
//	rootSequence->AddChild(sequence);
//	rootSequence->AddChild(selection);
//	rootSequence->AddChild(parallel);
//
//	
//
//	for (int i = 0; i < 5; ++i) {
//		std::cout << "~~~Rewind~~~\n";
//		rootSequence->Reset();
//		behaviourTimer = 0.0f;
//		distanceToTarget = rand() % 250;
//		BehaviourState state = BehaviourState_Ongoing;
//		std::cout << "We¡¯re going on an adventure !\n";
//		while (state == BehaviourState_Ongoing) {
//			state = rootSequence->Execute(1.0f); //fake dt
//		}
//		if (state == BehaviourState_Success) {
//			std::cout << "What a successful adventure !\n";
//		}
//		else if (state == BehaviourState_Failure) {
//			std::cout << "What a waste of time!\n";
//		}
//	}
//	std::cout << "All done!\n";
//
//}
//
//
////TODO: Find out how to pop multiple states in a row... Consult Rich...
//class PauseScreen :public PushdownState {
//public:
//	PauseScreen() :PushdownState() { name = "PauseScreen"; }
//	virtual PushdownResult PushdownUpdate(float dt, PushdownState** newState) override {
//		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::U)) {
//			return PushdownResult_Pop;
//		}
//		return PushdownResult_NoChange;
//	}
//	virtual void OnAwake() override {
//		std::cout << "Press U to unpause game!\n";
//	}
//};
//
//class GameScreen : public PushdownState {
//public:
//	GameScreen() :PushdownState() { name = "GameScreen"; }
//	PushdownResult PushdownUpdate(float dt, PushdownState **newState) override {
//		pauseReminder -= dt;
//		if (pauseReminder < 0) {
//			std::cout << "Coins mined: " << coinsMined << "\n";
//			std::cout << "Press P to pause game, or F1 to return to main menu!\n";
//			pauseReminder += 1.0f;
//		}
//		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::P)) {
//			*newState = new PauseScreen();
//			return PushdownResult_Push;
//		}
//		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F1)) {
//			std::cout << "Returning to main menu!\n";
//			return PushdownResult_Pop;
//		}
//		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::R)) {
//			std::cout << "Reset!\n";
//			PushdownReset();
//		}
//		if (rand() % 7 == 0) {
//			coinsMined++;
//		}
//		return PushdownResult_NoChange;
//	};
//	virtual void OnAwake() override {
//		std::cout << "Preparing to mine coins!\n";
//	}
//	virtual void PushdownReset() override {
//		coinsMined = 0;
//		pauseReminder = 1;
//	}
//protected:
//	int coinsMined = 0;
//	float pauseReminder = 1;
//};
//
//class PartBGame :public PushdownState {
//public:
//	PartBGame() :PushdownState() { name = "PartBGame"; }
//	virtual PushdownResult PushdownUpdate(float dt, PushdownState** newState) override {
//		if (g->UpdateGamePartB(dt)) {
//			return PushdownResult_Pop;
//		}
//		return PushdownResult_NoChange;
//	};
//	virtual void OnAwake() override {
//		g->ResetPartB();
//	}
//};
//
//class PartAGame :public PushdownState {
//public:
//	PartAGame() :PushdownState() { name = "PartAGame"; }
//	virtual PushdownResult PushdownUpdate(float dt, PushdownState** newState) override {
//		if (g->UpdateGamePartA(dt)) {
//			return PushdownResult_Pop;
//		}
//		return PushdownResult_NoChange;
//	};
//	virtual void OnAwake() override {
//		g->ResetPartA();
//	}
//};
//
//class IntroScreen : public PushdownState {
//public:
//	IntroScreen() :PushdownState() { name = "IntroScreen"; }
//	virtual PushdownResult PushdownUpdate(float dt, PushdownState** newState) override {
//		g->UpdateGameMenu(dt);
//
//		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM1)) {
//			*newState = new PartAGame();
//			return PushdownResult_Push;
//		}
//		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM2)) {
//			*newState = new PartBGame();
//			return PushdownResult_Push;
//		}
//		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::BACK)) {
//			return PushdownResult_Pop;
//		}
//		return PushdownResult_NoChange;
//	};
//	virtual void OnAwake() override {
//		
//	}
//};
//
//void TestPushdownAutomata(Window* w) {
//	PushdownMachine machine(new IntroScreen());
//	while (w->UpdateWindow()) {
//		float dt = w->GetTimer()->GetTimeDeltaSeconds();
//		if (!machine.Update(dt)) {
//			return;
//		}
//	}
//}

/*

The main function should look pretty familar to you!
We make a window, and then go into a while loop that repeatedly
runs our 'game' until we press escape. Instead of making a 'renderer'
and updating it, we instead make a whole game, and repeatedly update that,
instead. 

This time, we've added some extra functionality to the window class - we can
hide or show the 

*/
int main() {
	//TestStateMachine();
	//TestPathfinding();
	//DisplayPathfinding();
	//TestBehaviourTree();


	Window*w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);

	if (!w->HasInitialised()) {
		return -1;
	}

	srand(time(0));
	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	//TestPushdownAutomata(w);
	//return 0;

	g = TutorialGame::GetInstance();
	/*PushdownMachine machine(new IntroScreen());*/

	w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
			w->ShowConsole(false);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
			w->SetWindowPosition(0, 0);
		}

		w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

		g->PreUpdate();

		if (!g->UpdateGame(dt)) {
			break;
		}
	}
	

	Window::DestroyGameWindow();
}