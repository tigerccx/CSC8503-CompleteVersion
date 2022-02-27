#include "AIMoveAround.h"

#include "AIMoveAround.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/State.h"
#include "../../Common/Maths.h"
#include "../../Common/Vector2.h"

AIMoveAround::AIMoveAround()
{
	pathPoints.emplace_back(Vector2(0, 0));
	pathPoints.emplace_back(Vector2(1, 1));
	GetGO()->GetTransform().SetPosition(Vector3(0, 5, 0));
	Reset();
}

AIMoveAround::~AIMoveAround()
{
	stateMachine->Leave();
	delete stateMachine;
}

void AIMoveAround::SetWayPoints(const std::vector<Vector2>& points, float height)
{
	pathPoints = points;
	GetGO()->GetTransform().SetPosition(Vector3(0, 5, 0));
	Reset();
}

void AIMoveAround::Update(float dt)
{
	//Patrol(dt);
	//Chase(dt, transChaseTarget);
	stateMachine->Update(dt);
}

void AIMoveAround::Patrol(float dt)
{
	Vector3 posCur = GetGO()->GetTransform().GetPosition();
	Vector3 src = Vector3(pathPoints[idxFrom].x, posCur.y, pathPoints[idxFrom].y);
	Vector3 dst = Vector3(pathPoints[idxTo].x, posCur.y, pathPoints[idxTo].y);
	auto physicsObject = GetGO()->GetPhysicsObject();
	if ((posCur - dst).Length() < 0.02f) { //Update route
		Vector3 vCur = physicsObject->GetLinearVelocity();
		if (vCur.Length() > 0) {
			physicsObject->ApplyLinearImpulse(-vCur / physicsObject->GetInverseMass());
		}
		physicsObject->ClearForces();
		physicsObject->SetLinearVelocity(Vector3(0, 0, 0));

		idxFrom = (idxFrom + 1) % pathPoints.size();
		idxTo = (idxTo + 1) % pathPoints.size();
		src = Vector3(pathPoints[idxFrom].x, posCur.y, pathPoints[idxFrom].y);
		dst = Vector3(pathPoints[idxTo].x, posCur.y, pathPoints[idxTo].y);
	}
	MoveFromTo(dt, src, dst);
}

void AIMoveAround::MoveFromTo(float dt, Vector3 src, Vector3 dst)
{
	auto transform = GetGO()->GetTransform();
	auto physicsObject = GetGO()->GetPhysicsObject();

	Vector3 posCur = transform.GetPosition();
	Vector3 toDst = dst - posCur;
	Vector3 fromSrc = posCur - src;
	Vector3 src2Dst = dst - src;

	float lenSrc2Dst = src2Dst.Length();
	float lenToDst = toDst.Length();
	float lenFromDst = fromSrc.Length();

	toDst.Normalise();
	fromSrc.Normalise();
	src2Dst.Normalise();

	Vector3 vCur = physicsObject->GetLinearVelocity();

	Vector3 forward = Vector3((transform.GetMatrix() * Vector4(0, 0, -1, 0))).Normalised();

	Vector3 axis = Vector3::Cross(forward, toDst);
	float angle = RadiansToDegrees(acos(Vector3::Dot(forward, toDst)));

	if (angle > 1.0f) {
		auto rotCur = transform.GetOrientation();
		//rotCur = Quaternion(axis, angle) * rotCur;
		rotCur = rotCur * Quaternion(axis, angle);
		rotCur.Normalise();
		transform.SetOrientation(rotCur);
		Vector3 a = rotCur.ToEuler();
	}

	if (lenFromDst / lenSrc2Dst < 0.1f) {
		float startDis = (0.1f * lenSrc2Dst);
		float accTime = 2 * startDis / maxSpeed;
		float a = 2 * startDis / (accTime * accTime);
		Vector3 force = toDst * a / physicsObject->GetInverseMass();
		physicsObject->AddForce(force);
	}
	else if (lenToDst / lenSrc2Dst < 0.1f) {
		float speedCur = vCur.Length();
		float a = 2 * (0.1f * lenSrc2Dst - speedCur * breakTime) / (breakTime * breakTime);
		Vector3 force = toDst * a / physicsObject->GetInverseMass();
		physicsObject->AddForce(force);
	}
	else
	{
		float speedCur = vCur.Length();
		if (speedCur < maxSpeed) {
			float diffSpeed = maxSpeed - speedCur;
			float a = diffSpeed / dt;
			Vector3 force = toDst * a / physicsObject->GetInverseMass();
			physicsObject->AddForce(force);
		}
	}

}

void AIMoveAround::ComeToStop(float dt)
{
	auto physicsObject = GetGO()->GetPhysicsObject();
	Vector3 vCur = physicsObject->GetLinearVelocity();
	float speedCur = vCur.Length();
	if (speedCur > 0.05f) {
		Vector3 impulse = -vCur * (dt / fastBreakTime) / physicsObject->GetInverseMass();
		physicsObject->ApplyLinearImpulse(impulse);
	}
}

void AIMoveAround::ReturnToPatrol(float dt)
{
	auto transform = GetGO()->GetTransform();
	auto physicsObject = GetGO()->GetPhysicsObject();

	Vector3 posCur = transform.GetPosition();
	Vector3 src = posTemp;
	Vector3 dst = Vector3(pathPoints[idxFrom].x, posCur.y, pathPoints[idxFrom].y);
	if ((posCur - dst).Length() < 0.02f) { //Update route
		Vector3 vCur = physicsObject->GetLinearVelocity();
		if (vCur.Length() > 0) {
			physicsObject->ApplyLinearImpulse(-vCur / physicsObject->GetInverseMass());
		}
		physicsObject->ClearForces();
		physicsObject->SetLinearVelocity(Vector3(0, 0, 0));
	}
	else
	{
		MoveFromTo(dt, src, dst);
	}
}

void AIMoveAround::Chase(float dt, Transform* transChaseTarget)
{
	auto transform = GetGO()->GetTransform();
	auto physicsObject = GetGO()->GetPhysicsObject();

	if (transChaseTarget) {
		Vector3 posTargCur = transChaseTarget->GetPosition();
		Vector3 posCur = transform.GetPosition();
		posTargCur.y = posCur.y;
		Vector3 toTarg = posTargCur - posCur;
		float lenToTarg = toTarg.Length();
		toTarg = toTarg / lenToTarg;

		Vector3 forward = Vector3((transform.GetMatrix() * Vector4(0, 0, -1, 0))).Normalised();
		Vector3 axis = Vector3::Cross(forward, toTarg);
		float angle = RadiansToDegrees(acos(Vector3::Dot(forward, toTarg)));
		if (angle > 1.0f) {
			auto rotCur = transform.GetOrientation();
			//rotCur = Quaternion(axis, angle) * rotCur;
			rotCur = rotCur * Quaternion(axis, angle);
			rotCur.Normalise();
			transform.SetOrientation(rotCur);
			Vector3 a = rotCur.ToEuler();
		}

		Vector3 vCur = physicsObject->GetLinearVelocity();
		Vector3 vToTarg = forward * Vector3::Dot(forward, vCur);
		Vector3 vRest = vCur - vToTarg;

		Vector3 impulseCancelVRest = -vRest / physicsObject->GetInverseMass();
		physicsObject->ApplyLinearImpulse(impulseCancelVRest);

		float speedToTarg = vToTarg.Length();
		if (speedToTarg < maxSpeed) {
			float diffSpeed = maxSpeed - speedToTarg;
			float a = diffSpeed / dt;
			Vector3 force = toTarg * a / physicsObject->GetInverseMass();
			physicsObject->AddForce(force);
		}
	}
}

void AIMoveAround::Reset()
{
	auto transform = GetGO()->GetTransform();

	transform.SetPosition(Vector3(pathPoints[idxFrom].x, transform.GetPosition().y, pathPoints[idxFrom].y));

	stateMachine = new StateMachine();

	State* statePatrol = new State(
		[&](float dt, void* v) { this->Patrol(dt); },
		nullptr,
		nullptr,
		[&](void* v) { auto phys = this->GetGO()->GetPhysicsObject(); assert(phys); phys->ClearForces(); phys->SetLinearVelocity(Vector3(0, 0, 0)); }
	);
	statePatrol->name = "Patrol";
	State* stateChase = new State(
		[&](float dt, void* v) { this->Chase(dt, transChaseTarget); },
		nullptr,
		nullptr,
		[&](void* v) { auto phys = this->GetGO()->GetPhysicsObject(); assert(phys); phys->ClearForces();  phys->SetLinearVelocity(Vector3(0, 0, 0)); }
	);
	stateChase->name = "Chase";
	State* stateReturnToPatrol = new State(
		[&](float dt, void* v) { this->ReturnToPatrol(dt); },
		nullptr,
		[&](void* v) { this->posTemp = this->GetGO()->GetTransform().GetPosition(); },
		[&](void* v) { this->posTemp = Vector3(0, 0, 0); auto phys = this->GetGO()->GetPhysicsObject(); assert(phys); phys->ClearForces(); phys->SetLinearVelocity(Vector3(0, 0, 0)); }
	);
	stateReturnToPatrol->name = "ReturnToPatrol";

	//Need a transition state
	//State* stateTransition = new State(
	//	[&](float dt, void* v) { this->ComeToStop(dt); },
	//	nullptr
	//);

	lastState = AIMoveAroundState_Patrol;
	stateMachine->AddState(statePatrol);
	stateMachine->AddState(stateChase);
	stateMachine->AddState(stateReturnToPatrol);


	StateTransition* patrol2Chase = new StateTransition(
		[&](void* a, void* b)->bool {
			Vector3 posCur = transform.GetPosition();
			Vector3 posTarg = transChaseTarget->GetPosition();
			posTarg.y = posCur.y;
			if ((posTarg - posCur).Length() < senseRadius) {
				return true;
			}
			else {
				return false;
			}
		},
		nullptr, nullptr,
			statePatrol, stateChase,
			1.0f, [&](float dt) {
			std::cout << "Transition " << dt << std::endl;
			return false;
		}
		);
	StateTransition* chase2BackToPatrol = new StateTransition(
		[&](void* a, void* b)->bool {
			Vector3 posCur = transform.GetPosition();
			Vector3 posTarg = transChaseTarget->GetPosition();
			posTarg.y = posCur.y;
			float dis = (posTarg - posCur).Length();
			if (dis < caughtRadius || dis > giveUpRadius) {
				return true;
			}
			else {
				return false;
			}
		},
		nullptr, nullptr,
			stateChase, stateReturnToPatrol
			);
	StateTransition* backToPatrol2Patrol = new StateTransition(
		[&](void* a, void* b)->bool {
			Vector3 posCur = transform.GetPosition();
			Vector3 posTarg = Vector3(pathPoints[idxFrom].x, posCur.y, pathPoints[idxFrom].y);
			float dis = (posTarg - posCur).Length();
			if (dis < 0.05f) {
				return true;
			}
			else {
				return false;
			}
		},
		nullptr, nullptr,
			stateReturnToPatrol, statePatrol
			);

	stateMachine->AddTransition(patrol2Chase);
	stateMachine->AddTransition(chase2BackToPatrol);
	stateMachine->AddTransition(backToPatrol2Patrol);

	stateMachine->Enter();
}
