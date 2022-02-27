#pragma once
#include "../CSC8503Common/Component.h"
#include "../CSC8503Common/GameObject.h"
#include "../CSC8503Common/State.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/StateMachine.h"
#include "IImportable.h"

using namespace NCL;
using namespace NCL::CSC8503;

class AIMoveAround :
	public Component {
public:
    enum AIMoveAroundState {
        AIMoveAroundState_Patrol,
        AIMoveAroundState_Chase,
        AIMoveAroundState_BackToPatrol
    };

    AIMoveAround();
    ~AIMoveAround();

    void SetWayPoints(const std::vector<Vector2>& points, float height);

    virtual void Update(float dt) override;

    void Patrol(float dt);
    void MoveFromTo(float dt, Vector3 src, Vector3 dst);
    void ComeToStop(float dt);
    void ReturnToPatrol(float dt);
    void Chase(float dt, Transform* transChaseTarget);

    //std::vector<Vector3> pathPoints;
    std::vector<Vector2> pathPoints;

    float maxSpeed = 10.0f;
    float breakTime = 1.0f;
    float fastBreakTime = 0.2f;
    float turnTime = 0.1f;

    float senseRadius = 10.0f;
    float caughtRadius = 1.0f;
    float giveUpRadius = 20.0f;

    Transform* transChaseTarget;

protected:
    void Reset();

    int idxFrom = 0;
    int idxTo = 1;

    Vector3 posTemp;

    AIMoveAroundState lastState;

    StateMachine* stateMachine;
};