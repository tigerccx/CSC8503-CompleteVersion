#pragma once
#include "Constraint.h"
#include "../../Common/Quaternion.h"

using namespace NCL::Maths;

namespace NCL {
    namespace CSC8503 {
        class GameObject;
        
        class RotationConstraint :
            public Constraint
        {
        public:
            RotationConstraint(GameObject* a, Quaternion rot) {
                objectA = a;
                rotation = rot;
            }
            ~RotationConstraint() {}

            void UpdateConstraint(float dt) override;

        protected:
            GameObject* objectA;

            Quaternion rotation;
        };
    }
}

