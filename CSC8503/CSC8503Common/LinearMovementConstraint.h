#pragma once
#include "Constraint.h"
#include "../../Common/Vector3.h"

using namespace NCL::Maths;

namespace NCL {
    namespace CSC8503 {
        class GameObject;
        
        class LinearMovementConstraint :
            public Constraint
        {
        public:
            LinearMovementConstraint(GameObject* a, Vector3 enb) {
                objectA = a;
                enable = enb;
            }
            ~LinearMovementConstraint() {}

            void UpdateConstraint(float dt) override;

        protected:
            GameObject* objectA;
            Vector3 enable;
        };
    }
}

