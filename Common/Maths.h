/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include <algorithm>
#include "Exceptions.h"

namespace NCL {
	namespace Maths {
		class Vector2;
		class Vector3;
		class Plane;

		//It's pi(ish)...
		static const float		PI = 3.14159265358979323846f;

		//It's pi...divided by 360.0f!
		static const float		PI_OVER_360 = PI / 360.0f;

		//Radians to degrees
		inline float RadiansToDegrees(float rads) {
			return rads * 180.0f / PI;
		};

		//Degrees to radians
		inline float DegreesToRadians(float degs) {
			return degs * PI / 180.0f;
		};

		template<class T>
		inline T Clamp(T value, T min, T max) {
			if (value < min) {
				return min;
			}
			if (value > max) {
				return max;
			}
			return value;
		}

		Vector3 Clamp(const Vector3& a, const Vector3&mins, const Vector3& maxs);

		template<class T>
		inline T Lerp(const T& a, const T&b, float by) {
			return (a * (1.0f - by) + b*by);
		}

		void ScreenBoxOfTri(const Vector3& v0, const Vector3& v1, const Vector3& v2, Vector2& topLeft, Vector2& bottomRight);

		int ScreenAreaOfTri(const Vector3 &a, const Vector3 &b, const Vector3 & c);
		float FloatAreaOfTri(const Vector3 &a, const Vector3 &b, const Vector3 & c);

		float CrossAreaOfTri(const Vector3 &a, const Vector3 &b, const Vector3 & c);

		bool LinePlaneIntersection(const Vector3& dir0, const Vector3& pos0, const Vector3& normalPlane, const Vector3& ptPlane, Vector3& ptIntersect);
		inline bool LinePlaneIntersection(const Vector3& dir0, const Vector3& pos0, const Vector3& normalPlane, const Vector3& ptPlane, float& t);
		bool LineLineNormalProjection(const Vector3& dir0, const Vector3& pos0, const Vector3& dir1, const Vector3& pos1, const Vector3& normal,
			Vector3& ptIntersect);
		bool LineLineNormalProjection(const Vector3& dir0, const Vector3& pos0, const Vector3& dir1, const Vector3& pos1, const Vector3& normal,
			float& t);

		void ProjectPointOnPlane(const Vector3& pt, const Vector3& normalPlane, const Vector3& ptPlane, Vector3& ptOut);
		void ProjectPointOnPlane(const Vector3& pt, const Vector3& normalPlane, const float& dPlane, Vector3& ptOut);

		Vector3 ClosestPointOnLineSegment(const Vector3& pointLineA, const Vector3& pointLineB, const Vector3& point);
		Vector3 ClosestPointOnLine(const Vector3& dirLine, const Vector3& pointLine, const Vector3& point);
		bool ClosestPointsBetweenLineSegments(const Vector3& pointLine0A, const Vector3& pointLine0B, const Vector3& pointLine1A, const Vector3& pointLine1B,
			Vector3& point0Out, Vector3& point1Out);
		inline bool SegmentOverlap(const float& minA, const float& maxA, const float& minB, const float& maxB) {
			return !(maxA < minB || maxB < minA);
		}

		bool ApproxEqual(float a, float b, float e = 1e-6);
		bool ApproxEqual(Vector2 a, Vector2 b, float e = 1e-6);
		bool ApproxEqual(Vector3 a, Vector3 b, float e = 1e-6);
	}
}