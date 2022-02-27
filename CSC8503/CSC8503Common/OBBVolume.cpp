#include "OBBVolume.h"
#include "Transform.h"

void NCL::OBBVolume::GetPointSet(vector<Vector3>& ptSet, const Transform& transform) const
{
	Vector3 forward = transform.GetForward();
	Vector3 up = transform.GetUp();
	Vector3 right = Vector3::Cross(forward, up);
	Vector3 cent = transform.GetPosition();
	ptSet.emplace_back(cent + (up + forward + right) * halfSizes); //FTR
	ptSet.emplace_back(cent + (up + forward - right) * halfSizes); //FTL
	ptSet.emplace_back(cent + (up - forward + right) * halfSizes); //FBR
	ptSet.emplace_back(cent + (up - forward - right) * halfSizes); //FBL
	ptSet.emplace_back(cent + (-up + forward + right) * halfSizes); //BTR
	ptSet.emplace_back(cent + (-up + forward - right) * halfSizes); //BTL
	ptSet.emplace_back(cent + (-up - forward + right) * halfSizes); //BBR
	ptSet.emplace_back(cent + (-up - forward - right) * halfSizes); //BBL
}

void NCL::OBBVolume::GetPlaneSet(vector<Plane>& planeSet, const Transform& transform) const
{
	Vector3 forward = transform.GetForward();
	Vector3 up = transform.GetUp();
	Vector3 right = Vector3::Cross(forward, up);
	Vector3 cent = transform.GetPosition();

	planeSet.emplace_back(Plane(forward, -Vector3::Dot((cent + forward * halfSizes), forward))); //OBBVolumePointSet_Front,
	planeSet.emplace_back(Plane(-forward, -Vector3::Dot((cent - forward * halfSizes), -forward))); //OBBVolumePointSet_Back,
	planeSet.emplace_back(Plane(up, -Vector3::Dot((cent + up * halfSizes), up)));//OBBVolumePointSet_Top,
	planeSet.emplace_back(Plane(-up, -Vector3::Dot((cent - up * halfSizes), -up)));//OBBVolumePointSet_Bottom,
	planeSet.emplace_back(Plane(right, -Vector3::Dot((cent + right * halfSizes), right)));//OBBVolumePointSet_Right,
	planeSet.emplace_back(Plane(-right, -Vector3::Dot((cent - right * halfSizes), -right)));//OBBVolumePointSet_Left
}
