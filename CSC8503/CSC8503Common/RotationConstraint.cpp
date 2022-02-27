#include "RotationConstraint.h"
#include "../CSC8503Common//GameObject.h"

void NCL::CSC8503::RotationConstraint::UpdateConstraint(float dt)
{
	Quaternion rotationCur = objectA->GetTransform().GetOrientation();
	Quaternion rotationBetween = Quaternion::Between(rotation, rotationCur);

	if (rotationBetween.ToVector4().LengthSquared() > 0.0f) {
		/*PhysicsObject* physA = objectA->GetPhysicsObject();
		Vector3 angularVelocity = physA->GetAngularVelocity();
		float inverseMass = physA->GetInverseMass();

		if (inverseMass > 0.0f) {
			Vector3 inertiaA = Vector3::Cross(physA->GetInertiaTensor() * Vector3::Cross(p.localA, p.normal), relativeA);
			Vector3 impulse = physA->GetInertiaTensor() ;
			physA->ApplyAngularImpulse
		}*/
		PhysicsObject* physA = objectA->GetPhysicsObject();
		physA->SetAngularVelocity(Vector3(0,0,0));
	}
}
