#include "LinearMovementConstraint.h"
#include "../CSC8503Common//GameObject.h"

void NCL::CSC8503::LinearMovementConstraint::UpdateConstraint(float dt)
{
	PhysicsObject* physA = objectA->GetPhysicsObject();
	Vector3 v = physA->GetLinearVelocity();
	v = (Vector3(1,1,1) - enable) * v;
	physA->SetLinearVelocity(v);
}
