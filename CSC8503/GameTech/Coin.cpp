#include "Coin.h"
#include "../CSC8503Common/GameObject.h"

//void Coin::OnCollisionBegin(GameObject* otherObject)
//{
//	if (!isCollected) {
//		auto controllableCharacter = otherObject->GetComponent<ControllableCharacter>();
//		if (controllableCharacter!=nullptr) {
//			GetGO()->GetRenderObject()->SetColour(Vector4(0, 0, 0, 0));
//			GetGO()->SetObjectLayer("Ghost");
//		}
//	}
//}
int Coin::coinCount = 0;

void Coin::Update(float dt)
{
	Vector3 posCollector = goCollector->GetTransform().GetPosition();
	Vector3 pos = GetGO()->GetTransform().GetPosition();
	if (!collected && (pos - posCollector).Length() < GetGO()->GetTransform().GetScale().x) {
		GetGO()->GetRenderObject()->SetColour(Vector4(0, 0, 0, 0));
		coinCount -= 1;
		collected = true;
	}
}
