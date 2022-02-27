#pragma once
class UIElementBase {
public:
	UIElementBase(float tempDur = -1) :tempDur(tempDur) {}

	bool isActive = true;
	void Update(float dt) {
		if (tempDur > 0) {
			__tempDurCur += dt;
		}
	}
	bool IsExpired() { return(tempDur>0 && __tempDurCur > tempDur); }
	virtual void Render() = 0;

protected:
	float tempDur;
	float __tempDurCur = 0.0f;
};