/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "Maths.h"
#include "../Common/Vector2.h"
#include "../Common/Vector3.h"
#include "../Common/Plane.h"
#include "../Common/Matrix3.h"

namespace NCL {
	namespace Maths {
		void ScreenBoxOfTri(const Vector3& v0, const Vector3& v1, const Vector3& v2, Vector2& topLeft, Vector2& bottomRight) {
			topLeft.x = std::min(v0.x, std::min(v1.x, v2.x));
			topLeft.y = std::min(v0.y, std::min(v1.y, v2.y));

			bottomRight.x = std::max(v0.x, std::max(v1.x, v2.x));
			bottomRight.y = std::max(v0.y, std::max(v1.y, v2.y));
		}

		int ScreenAreaOfTri(const Vector3 &a, const Vector3 &b, const Vector3 & c) {
			int area =(int) (((a.x * b.y) + (b.x * c.y) + (c.x * a.y)) -
				((b.x * a.y) + (c.x * b.y) + (a.x * c.y)));
			return (area >> 1);
		}

		float FloatAreaOfTri(const Vector3 &a, const Vector3 &b, const Vector3 & c) {
			float area = ((a.x * b.y) + (b.x * c.y) + (c.x * a.y)) -
				((b.x * a.y) + (c.x * b.y) + (a.x * c.y));
			return (area * 0.5f);
		}

		float CrossAreaOfTri(const Vector3 &a, const Vector3 &b, const Vector3 & c) {
			Vector3 area = Vector3::Cross(a - b, a - c);
			return area.Length() * 0.5f;
		}

		//void ProjectPointOnPlane(const Plane& plane, const Vector3& pt, Vector3& ptOut)
		//{
		//	float disPlane = -plane.GetDistance();
		//	Vector3 normPlane = plane.GetNormal();

		//	float disPt = Vector3::Dot(normPlane, pt);
		//	ptOut = pt + normPlane * (disPlane - disPt);
		//}
	

		Vector3 Clamp(const Vector3& a, const Vector3&mins, const Vector3& maxs) {
			return Vector3(
				Clamp(a.x, mins.x, maxs.x),
				Clamp(a.y, mins.y, maxs.y),
				Clamp(a.z, mins.z, maxs.z)
			);
		}

		bool LinePlaneIntersection(const Vector3& dir0, const Vector3& pos0, const Vector3& normalPlane, const Vector3& ptPlane, Vector3& ptIntersect)
		{
			float t;
			LinePlaneIntersection(dir0, pos0, normalPlane, ptPlane, t);
			ptIntersect = pos0 + dir0 * t;
			return true;
		}

		bool LinePlaneIntersection(const Vector3& dir0, const Vector3& pos0, const Vector3& normalPlane, const Vector3& ptPlane, float& t)
		{
			float dotLinePlane = (Vector3::Dot(dir0, normalPlane));
			if (abs(dotLinePlane) < 1e-6) { //Parallel
				return false;
			}
			t = (Vector3::Dot((ptPlane - pos0), normalPlane)) / dotLinePlane; //Check Edge Parallel
			return true;
		}

		//Return projection point along normal of line0
		bool LineLineNormalProjection(const Vector3& dir0, const Vector3& pos0, const Vector3& dir1, const Vector3& pos1, const Vector3& normal,
			Vector3& ptIntersect) {
			Vector3 normPlane = Vector3::Cross(dir1, normal).Normalised();
			return LinePlaneIntersection(dir0, pos0, normPlane, pos1, ptIntersect);
			//Vector3 planePt = pos1;
			//Vector3 dirLine = dir0;
			//Vector3 linePt = pos0;
			//float dotLinePlane = (Vector3::Dot(dirLine, normPlane));
			//if (abs(dotLinePlane) < 1e-6) { //Parallel
			//	return false;
			//}
			//float t = (Vector3::Dot((planePt - linePt), normPlane)) / dotLinePlane; //Check Edge Parallel
			//ptIntersect = linePt + dirLine * t;
			//return true;
		}
		bool LineLineNormalProjection(const Vector3& dir0, const Vector3& pos0, const Vector3& dir1, const Vector3& pos1, const Vector3& normal, float& t)
		{
			Vector3 normPlane = Vector3::Cross(dir1, normal).Normalised();
			return LinePlaneIntersection(dir0, pos0, normPlane, pos1, t);
		}
		void ProjectPointOnPlane(const Vector3& pt, const Vector3& normalPlane, const Vector3& ptPlane, Vector3& ptOut)
		{
			ProjectPointOnPlane(pt, normalPlane, Vector3::Dot(normalPlane, ptPlane), ptOut);
		}

		void ProjectPointOnPlane(const Vector3& pt, const Vector3& normalPlane, const float& dPlane, Vector3& ptOut)
		{
			float dotPtPlane = Vector3::Dot(pt, normalPlane);
			float dis = dPlane + dotPtPlane;
			ptOut = pt - normalPlane * dis;
		}

		Vector3 ClosestPointOnLineSegment(const Vector3& pointLineA, const Vector3& pointLineB, const Vector3& point)
		{
			Vector3 AB = pointLineB - pointLineA;
			float t = Vector3::Dot(point - pointLineA, AB) / Vector3::Dot(AB, AB);
			return pointLineA + AB * Clamp(t, 0.0f, 1.0f); // saturate(t) can be written as: min((max(t, 0), 1)
		}

		Vector3 ClosestPointOnLine(const Vector3& dirLine, const Vector3& pointLine, const Vector3& point)
		{
			float t = Vector3::Dot(point - pointLine, dirLine) / Vector3::Dot(dirLine, dirLine);
			return pointLine + dirLine * t;
		}

		bool ClosestPointsBetweenLineSegments(const Vector3& pointLine0A, const Vector3& pointLine0B, const Vector3& pointLine1A, const Vector3& pointLine1B,
			Vector3& point0Out, Vector3& point1Out)
		{
			//line 0
			Vector3 normal0 = pointLine0B - pointLine0A;
			float len0 = normal0.Length();
			normal0 = normal0 / len0;

			//line 1
			Vector3 normal1 = pointLine1B - pointLine1A;
			float len1 = normal1.Length();
			normal1 = normal1 / len1;

			Vector3 cross01 = Vector3::Cross(normal0, normal1);

			Vector3 v1 = normal0;
			Vector3 v2 = -normal1;
			Vector3 v3 = cross01;
			Vector3 v4 = pointLine1A - pointLine0A;

			float element[9] = { v1.x,v1.y,v1.z,
									v2.x,v2.y,v2.z,
									v3.x,v3.y,v3.z };
			Matrix3 mat = Matrix3(element);
			Vector3 t = Matrix3::Solve(mat, v4);

			Vector3 a = mat * t;
			Vector3 c = mat.GetRow(0);
			float b = Vector3::Dot(c, Vector3(t.x, t.x, t.x));

			if (!isfinite(t.x) || !isfinite(t.y) || !isfinite(t.z))
				return false;

			float t0 = t.x;
			float t1 = t.y;
			float t2 = t.z;

			float t0Clamp = Clamp(t0, 0.0f, len0);
			float t1Clamp = Clamp(t1, 0.0f, len1);

			if (t0 != t0Clamp) { //Clamped
				point0Out = pointLine0A + normal0 * t0Clamp;
				point1Out = ClosestPointOnLineSegment(pointLine1A, pointLine1B, point0Out);
			}
			else if (t1 != t1Clamp) {
				point1Out = pointLine1A + normal1 * t1Clamp;
				point0Out = ClosestPointOnLineSegment(pointLine0A, pointLine0B, point1Out);
			}
			else {
				point0Out = pointLine0A + normal0 * t0;
				point1Out = pointLine1A + normal1 * t1;
			}

			return true;
		}
		bool ApproxEqual(float a, float b, float e)
		{
			return abs(a - b) < e;
		}
		bool ApproxEqual(Vector2 a, Vector2 b, float e)
		{
			return (a-b).Length() < e;
		}
		bool ApproxEqual(Vector3 a, Vector3 b, float e)
		{
			return (a - b).Length() < e;
		}
	}
}