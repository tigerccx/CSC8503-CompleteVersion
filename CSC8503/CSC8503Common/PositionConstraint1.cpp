#include "PositionConstraint1.h"
#include "../CSC8503Common//GameObject.h"

void NCL::CSC8503::PositionConstraint1::UpdateConstraint(float dt)
{
	//Vector3 posA = objectA->GetTransform().GetPosition();
	Vector3 relativePos = objectA->GetTransform().GetPosition() - objectB->GetTransform().GetPosition();
	
	float currentDistance = relativePos.Length();
	float offset = distance - currentDistance;
	
	PhysicsObject* physA = objectA->GetPhysicsObject();
	PhysicsObject* physB = objectB->GetPhysicsObject();

	assert(physA);
	assert(physB);

	if (abs(offset) > 0.0f) {
		Vector3 offsetDir = relativePos.Normalised();


		Vector3 relativeVelocity = physA->GetLinearVelocity() - physB->GetLinearVelocity();

		float constraintMass = physA->GetInverseMass() + physB->GetInverseMass();

		if (constraintMass > 0.0f) {
			float velocityDot = Vector3::Dot(relativeVelocity, offsetDir); //Calculate the velocity required to counter out axial-directional movement
			float biasFactor = 0.01f;
			float bias = -(biasFactor / dt) * offset; 

			float lambda = -(velocityDot + bias) / constraintMass; //Parse calculated velocity as impulse magnitude

			Vector3 aImpulse = offsetDir * lambda; //Create impulse
			Vector3 bImpulse = -offsetDir * lambda;

			physA->ApplyLinearImpulse(aImpulse); //Apply impulse
			physB->ApplyLinearImpulse(bImpulse);
			
		}
	}

	float linearSpeedB = physB->GetLinearVelocity().Length();
	if (linearSpeedB < scale) {
		Vector3 a = Vector3(0, 0, 1);
		float constraintMass = physA->GetInverseMass() + physB->GetInverseMass();
		Vector3 impulse = Vector3::Cross(a, relativePos.Normalised()).Normalised() * (scale - linearSpeedB) / constraintMass;
		physB->ApplyLinearImpulse(impulse);
	}
}


