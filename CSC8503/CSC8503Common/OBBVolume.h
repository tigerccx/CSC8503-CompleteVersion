#pragma once
#include "CollisionVolume.h"
#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"
#include "../../Common/Matrix3.h"
#include "../../Common/Matrix4.h"
#include "Transform.h"
#include "../../Common/Plane.h"

using namespace NCL::CSC8503;

namespace NCL {
	namespace CSC8503 {
		class Transform;
	}

	enum OBBVolumePointSet {
		OBBVolumePointSet_FTR,
		OBBVolumePointSet_FTL,
		OBBVolumePointSet_FBR,
		OBBVolumePointSet_FBL,
		OBBVolumePointSet_BTR,
		OBBVolumePointSet_BTL,
		OBBVolumePointSet_BBR,
		OBBVolumePointSet_BBL
	};

	enum OBBVolumePlaneSet {
		OBBVolumePlaneSet_Front,
		OBBVolumePlaneSet_Back,
		OBBVolumePlaneSet_Top,
		OBBVolumePlaneSet_Bottom,
		OBBVolumePlaneSet_Right,
		OBBVolumePlaneSet_Left
	};

	class OBBVolume : CollisionVolume
	{
	public:
		OBBVolume(const Maths::Vector3& halfDims) {
			type		= VolumeType::OBB;
			halfSizes	= halfDims;
		}
		~OBBVolume() {}

		Maths::Vector3 GetHalfDimensions() const {
			return halfSizes;
		}

		void GetPointSet(vector<Vector3>& ptSet, const Transform& transform) const;
		void GetPlaneSet(vector<Plane>& planeSet, const Transform& transform) const;
	protected:
		Maths::Vector3 halfSizes;
	};
}

