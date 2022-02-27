#pragma once
#include "Constraint.h"

namespace NCL {
    namespace CSC8503 {
        class GameObject;

        class PositionConstraint1 :
            public Constraint
        {
        public:
            PositionConstraint1(GameObject* a, GameObject* b, float d, float s) {
                objectA = a;
                objectB = b;
                distance = d;
                scale = s;
            }
            ~PositionConstraint1() {}

            void UpdateConstraint(float dt) override;

        protected:
            GameObject* objectA;
            GameObject* objectB;

            float distance;
            float scale;
        };
    }
}

