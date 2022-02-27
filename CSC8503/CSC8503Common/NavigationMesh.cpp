#include "NavigationMesh.h"
#include "../../Common/Assets.h"
#include "../../Common/Maths.h"
#include <fstream>
#include "../Common/Exceptions.h"

#include "../CSC8503/CSC8503Common/Debug.h"
#include "../Common/Maths.h"
#include <cassert>

using namespace NCL;
using namespace CSC8503;
using namespace std;

NavigationMesh::NavigationMesh()
{
}


//NOTE: NavTri (In Unity) has been modified that neighbours and indices have the following relationship:
//indices<->neighbour: [0,1]<->[0], [1,2]<->[1], [2,0]<->[2]
//The file should contain the same relation, or pathway smoothing will fail!
NavigationMesh::NavigationMesh(const std::string&filename)
{
	ifstream file(Assets::DATADIR + filename);

	if (!file) {
		throw LoadErrorException();
	}

	int numVertices = 0;
	int numIndices	= 0;

	file >> numVertices;
	file >> numIndices;

	for (int i = 0; i < numVertices; ++i) {
		Vector3 vert;
		file >> vert.x;
		file >> vert.y;
		file >> vert.z;

		allVerts.emplace_back(vert);
	}

	allTris.resize(numIndices / 3);

	for (int i = 0; i < allTris.size(); ++i) {
		NavTriNode* tri = &allTris[i];
		file >> tri->indices[0];
		file >> tri->indices[1];
		file >> tri->indices[2];

		tri->centroid = allVerts[tri->indices[0]] +
			allVerts[tri->indices[1]] +
			allVerts[tri->indices[2]];

		tri->centroid = allTris[i].centroid / 3.0f;

		tri->triPlane = Plane::PlaneFromTri(allVerts[tri->indices[0]],
			allVerts[tri->indices[1]],
			allVerts[tri->indices[2]]);

		tri->area = Maths::CrossAreaOfTri(allVerts[tri->indices[0]], allVerts[tri->indices[1]], allVerts[tri->indices[2]]);
	}
	for (int i = 0; i < allTris.size(); ++i) {
		NavTriNode* tri = &allTris[i];
		for (int j = 0; j < 3; ++j) {
			int index = 0;
			file >> index;
			if (index != -1) {
				tri->neighbours[j] = &allTris[index];
				tri->costs[j] = (tri->centroid- tri->neighbours[j]->centroid).Length(); 
				//NOTE: Here we use the distance between 2 centroids as cost. This can lead to minor inaccuracy but it should work fine...
			}
			else
			{
				tri->neighbours[j] = nullptr;
			}
		}
	}
}

NavigationMesh::~NavigationMesh()
{
}

bool NCL::CSC8503::NavigationMesh::FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath)
{
	NavMeshPathFindState state = FindPath1(from, to, outPath);
	if (state == NavMeshPathFindState_Success) {
		return true;
	}
	else
	{
		return false;
	}
}

NavMeshPathFindState NavigationMesh::FindPath1(const Vector3& from, const Vector3& to, NavigationPath& outPath) {
	NavTriNode* startNode = GetTriForPosition(from);
	NavTriNode* endNode	= GetTriForPosition(to);

	if (!startNode) {
		return NavMeshPathFindState_FromNotOnGrid;
	}
	else if(!endNode)
	{
		return NavMeshPathFindState_ToNotOnGrid;
	}
	if (startNode == endNode) {
		outPath.PushWaypoint(to);
		outPath.PushWaypoint(from);
		return NavMeshPathFindState_Success;
	}

	NavTriNodeOpenList  openList(CmpNavTriNodePtr);
	NavTriNodeCloseList  closedList;

	openList.insert(startNode);
	startNode->f = 0;
	startNode->g = 0;
	startNode->parent = nullptr;

	NavTriNode* currentBestNode = nullptr;

	while (!openList.empty()) {
		currentBestNode = RemoveBestNode(openList);

		if (currentBestNode == endNode) {			//we've found the path!
			if (!enableSmoothing) {
				/*outPath.PushWaypoint(to);
				NavTriNode* node = endNode->parent;
				while (node != startNode) {
					outPath.PushWaypoint(node->centroid);
					node = node->parent;
				}
				outPath.PushWaypoint(from);*/
				PathwayEdgeCentSmoothing(endNode, from, to, outPath);
			}
			else
			{
				//PathwaySGDSmoothing(endNode, from, to, outPath);
				PathwayMyWierdSmoothing(endNode, from, to, outPath);
				//PathwayFunnelSmoothing(endNode, from, to, outPath);
			}

			return NavMeshPathFindState_Success;
		}
		else {
			for (int i = 0; i < 3; ++i) {
				NavTriNode* neighbour = currentBestNode->neighbours[i];
				if (!neighbour) { //might not be connected...
					continue;
				}
				bool inClosed = NodeInCloseList(neighbour, closedList);
				if (inClosed) {
					continue; //already discarded this neighbour...
				}

				float h = Heuristic(neighbour, endNode);
				float g = currentBestNode->g + currentBestNode->costs[i];
				float f = h + g;

				bool inOpen = NodeInOpenList(neighbour, openList);

				if (!inOpen) { //first time we've seen this neighbour
					openList.insert(neighbour);
				}
				if (!inOpen || f < neighbour->f) {//might be a better route to this neighbour
					neighbour->parent = currentBestNode;
					neighbour->f = f;
					neighbour->g = g;
				}
			}
			closedList.insert(currentBestNode);
		}
	}
	return NavMeshPathFindState_UnableToFindPath; //open list emptied out with no path!
}

Vector3 NavigationMesh::GetNearestVertOnMesh(const Vector3& point) {
	float len = FLT_MAX;
	Vector3 vertNearest;
	for (auto& vert : allVerts) {
		float lenCur = (vert - point).LengthSquared();
		if (lenCur < len) {
			vertNearest = vert;
			len = lenCur;
		}
	}
	return vertNearest;
}

/*
NOTE: 
If you have triangles on top of triangles in a full 3D environment, you'll need to change this slightly,
as it is currently ignoring height. You might find tri/plane raycasting is handy.
*/

NavTriNode* NavigationMesh::GetTriForPosition(const Vector3& pos) {
	for (NavTriNode& t : allTris) {
		Vector3 planePoint = t.triPlane.ProjectPointOntoPlane(pos);

		float ta = Maths::CrossAreaOfTri(allVerts[t.indices[0]], allVerts[t.indices[1]], planePoint);
		float tb = Maths::CrossAreaOfTri(allVerts[t.indices[1]], allVerts[t.indices[2]], planePoint);
		float tc = Maths::CrossAreaOfTri(allVerts[t.indices[2]], allVerts[t.indices[0]], planePoint);

		float areaSum = ta + tb + tc;

		if (abs(areaSum - t.area)  > 0.001f) { //floating points are annoying! Are we more or less inside the triangle?
			continue;
		}
		return &t;
	}
	return nullptr;
}

bool NCL::CSC8503::NavigationMesh::NodeInOpenList(NavTriNode* n, NavTriNodeOpenList& list) const
{
	auto i = std::find_if(list.begin(), list.end(), [&](NavTriNode* a)->bool { return a == n; });
	return i == list.end() ? false : true;
}

bool NCL::CSC8503::NavigationMesh::NodeInCloseList(NavTriNode* n, NavTriNodeCloseList& list) const
{
	auto i = std::find(list.begin(), list.end(), n);
	return i == list.end() ? false : true;
}

NavTriNode* NCL::CSC8503::NavigationMesh::RemoveBestNode(NavTriNodeOpenList& list) const
{
	auto bestI = list.begin();
	NavTriNode* bestNode = *bestI;
	list.erase(bestI);

	return bestNode;
}

float NCL::CSC8503::NavigationMesh::Heuristic(NavTriNode* hNode, NavTriNode* endNode) const
{
	return (hNode->centroid - endNode->centroid).Length();
}

Vector3 ElementCalc0(const float& t, const Vector3& v0, const Vector3& v1) {
	return v0 * (1 - t) + v1 * t;
}

Vector3 ElementCalc1(Vector3 v0, Vector3 v1, Vector3 v2) {
	return -(v0 * (-4) + (v1 + v2) * 2);
}

void NCL::CSC8503::NavigationMesh::PathwayEdgeCentSmoothing(NavTriNode* nEnd, Vector3 from, Vector3 to, NavigationPath& outPath)
{
	outPath.Clear();

	NavTriNode* node = nEnd;
	vector<NavTriNode*> nodes;
	while (node->parent) {
		nodes.emplace_back(node);
		node = node->parent;
	}
	nodes.emplace_back(node);
	std::reverse(nodes.begin(), nodes.end());

	
	outPath.PushWaypoint(from);
	//vector<NavTriEdge> edges;

	for (int i = 0; i < nodes.size() - 1; ++i) {
		NavTriNode* nodeCur = nodes[i];
		NavTriNode* nodeNext = nodes[i + 1];
		int idxNeighbour = -1;
		for (int i = 0; i < 3; ++i) {
			if (nodeNext == nodeCur->neighbours[i]) {
				idxNeighbour = i;
				break;
			}
		}
		if (idxNeighbour < 0) {
			throw NotImplementedException(); //Maybe in offmesh jump link, this might happen (or in some accidents...)
		}

		NavTriEdge edgeCross;
		nodeCur->GetEdgeFromNeighbour(idxNeighbour, edgeCross);
		//edges.emplace_back(edgeCross);
		outPath.PushWaypoint((allVerts[edgeCross.i0]+ allVerts[edgeCross.i1])*0.5f);
	}
	outPath.PushWaypoint(to);
}

//NOTE: Again!!
//NavTri (In Unity) has been modified that neighbours and indices have the following relationship:
//indices<->neighbour: [0,1]<->[0], [1,2]<->[1], [2,0]<->[2]
//The NavigationMesh should contain the same relation, or pathway smoothing will fail!
void NCL::CSC8503::NavigationMesh::PathwaySGDSmoothing(NavTriNode* nEnd, Vector3 from, Vector3 to, NavigationPath& outPath)
{
	outPath.Clear();

	NavTriNode* node = nEnd;
	vector<NavTriNode*> nodes;
	while (node->parent) {
		nodes.emplace_back(node);
		node = node->parent;
	}
	nodes.emplace_back(node);
	std::reverse(nodes.begin(), nodes.end());

	vector<NavTriEdge> edges;

	for (int i = 0; i < nodes.size() - 1; ++i) {
		NavTriNode* nodeCur = nodes[i];
		NavTriNode* nodeNext = nodes[i + 1];
		int idxNeighbour = -1;
		for (int i = 0; i < 3; ++i) {
			if (nodeNext == nodeCur->neighbours[i]) {
				idxNeighbour = i;
				break;
			}
		}
		if (idxNeighbour < 0) {
			throw NotImplementedException(); //Maybe in offmesh jump link, this might happen (or in some accidents...)
		}

		NavTriEdge edgeCross;
		nodeCur->GetEdgeFromNeighbour(idxNeighbour, edgeCross);
		edges.emplace_back(edgeCross);
	}

	vector<float> ts(edges.size(), 0.5);
	vector<Vector3> temp0(edges.size(), Vector3(0,0,0));
	vector<Vector3> temp1(edges.size(), Vector3(0, 0, 0));
	float stepScale = 0.0002f;
	float dis = 0.0f;
	int updates = 100;
	for (int step = 0; step < updates; ++step) {
		dis = 0.0f;
		for (int i = 0; i < edges.size(); ++i) {
			temp0[i] = ElementCalc0(ts[i], allVerts[edges[i].i0], allVerts[edges[i].i1]);
			if (i == 0) {
				dis += (temp0[i] - from).Length();
			}
			else {
				dis += (temp0[i] - temp0[i-1]).Length();
			}
			if(i == edges.size() - 1)
			{
				dis += (temp0[i] - to).Length();
			}
		}

		for (int i = 0; i < edges.size(); ++i) {
			if (i == 0) {
				temp1[i] = ElementCalc1(temp0[i], temp0[i + 1], from);
			}
			else if(i==edges.size()-1)
			{
				temp1[i] = ElementCalc1(temp0[i], temp0[i - 1], to);
			}
			else
			{
				temp1[i] = ElementCalc1(temp0[i], temp0[i - 1], temp0[i + 1]);
			}
		}
		for (int i = 0; i < edges.size(); ++i) {
			ts[i] = Maths::Clamp(ts[i] - stepScale * Vector3::Dot(temp1[i], allVerts[edges[i].i1] - allVerts[edges[i].i0]), 0.0f, 1.0f);
			//ts[i] = ts[i] - stepScale * Vector3::Dot(temp1[i], allVerts[edges[i].i1] - allVerts[edges[i].i0]);
		}
	}

	outPath.PushWaypoint(from);
	for (int i = 0; i < edges.size(); ++i) {
		outPath.PushWaypoint(ElementCalc0(ts[i], allVerts[edges[i].i0], allVerts[edges[i].i1]));
	}
	outPath.PushWaypoint(to);

	outPath.Flip();
}
void NCL::CSC8503::NavigationMesh::PathwayMyWierdSmoothing(NavTriNode* nEnd, Vector3 from, Vector3 to, NavigationPath& outPath)
{
	std::vector<Vector3> pts;
	vector<NavTriNode*> nodes;
	nodes.emplace_back(nEnd);
	pts.emplace_back(to);
	NavTriNode* node = nEnd->parent;
	while (node->parent != nullptr) {
		nodes.emplace_back(node);
		pts.emplace_back(node->centroid);
		node = node->parent;
	}
	nodes.emplace_back(node);
	//pts.emplace_back(from);
	std::reverse(pts.begin(), pts.end());
	std::reverse(nodes.begin(), nodes.end());

	//for (int i = 0; i < pts.size(); ++i) {
	//	if(i==0)
	//		Debug::DrawLine(pts[i], from, Vector4(0,0,1,1), 5.0f);
	//	else
	//	{
	//		Debug::DrawLine(pts[i], pts[i-1], Vector4(0, 0, 1, 1), 5.0f);
	//	}
	//}

	vector<NavTriEdge> edges;

	for (int i = 0; i < nodes.size() - 1; ++i) {
		NavTriNode* nodeCur = nodes[i];
		NavTriNode* nodeNext = nodes[i + 1];
		int idxNeighbour = -1;
		for (int i = 0; i < 3; ++i) {
			if (nodeNext == nodeCur->neighbours[i]) {
				idxNeighbour = i;
				break;
			}
		}
		if (idxNeighbour < 0) {
			throw NotImplementedException(); //Maybe in offmesh jump link, this might happen (or in some accidents...)
		}

		NavTriEdge edgeCross;
		nodeCur->GetEdgeFromNeighbour(idxNeighbour, edgeCross);
		edges.emplace_back(edgeCross);
	}

	int iBeg, iCur;
	iBeg = 0;
	Vector3 wayPoint = from;
	outPath.PushWaypoint(wayPoint);
	for (int iCur = 0; iCur < pts.size();) {
		bool outOfLine = false;
		Vector3& ptLine = wayPoint;

		//int iOutOfLine;
		float t = 0.0f;

		vector<Vector3> wayPoints;
		for (int iSub = iBeg; iSub <= iCur; ++iSub) {
			Vector3 dirLine = (pts[iCur] - wayPoint).Normalised();
			Vector3 dirEdge = (allVerts[edges[iSub].i1] - allVerts[edges[iSub].i0]);
			float edgeLength = dirEdge.Length();
			dirEdge = dirEdge / edgeLength;
			Vector3 ptEdge = allVerts[edges[iSub].i0];
			Vector3 ptOnEdge;
			assert(Maths::LineLineNormalProjection(dirEdge, ptEdge, dirLine, ptLine, Vector3::Cross(dirLine, dirEdge).Normalised(), ptOnEdge));
			//if (iSub == 0) {
			//	Debug::DrawLine(ptEdge+Vector3(0,0.01,0), allVerts[edges[iSub].i1] + Vector3(0, 0.01, 0), Vector4(1, 0, 0, 1), 5.0f);
			//	/*Debug::DrawLine(ptLine, pts[iSub], Vector4(0, 0, 1, 1), 5.0f);
			//	Debug::DrawLine(ptEdge, ptEdge+Vector3::Cross(dirLine, dirEdge).Normalised()*2, Vector4(0, 1, 0, 1), 5.0f);*/
			//}
			float edgeLengthMod = (edgeLength - 2 * smoothToBoundaryDistance);
			t = (Vector3::Dot(ptOnEdge - ptEdge, dirEdge) - smoothToBoundaryDistance) / edgeLengthMod;
			//if (iSub == 0&&iCur==0) {
			//	Debug::DrawLine(ptEdge, ptOnEdge, Vector4(1, 0, 0, 1), 5.0f);
			//}
			if (t < 0.0f || t>1.0f) {
				outOfLine = true;
				t = Maths::Clamp(t, 0.0f, 1.0f);
				t = t* (edgeLengthMod+ smoothToBoundaryDistance)/edgeLength;
				Vector3 wayPointsTmp = allVerts[edges[iSub].i0] * (1 - t) + allVerts[edges[iSub].i1] * t;
				bool merge = false;
				for (const Vector3& wp : wayPoints) {
					if ((wp - wayPointsTmp).LengthSquared() < 1e-5) {
						merge = true;
						break;
					}
				}
				if(!merge)
					wayPoints.emplace_back(wayPointsTmp);
				wayPoint = wayPointsTmp;
			}
		}

		if (outOfLine) {
			for (int i = 0; i < wayPoints.size(); ++i) {
				outPath.PushWaypoint(wayPoints[i]);
			}
			wayPoint = wayPoints[wayPoints.size() - 1];

			++iCur;
			iBeg = iCur;
		}
		else
		{
			++iCur;
		}
	}
	outPath.PushWaypoint(to);

	outPath.Flip();
}
void NCL::CSC8503::NavigationMesh::PathwayFunnelSmoothing(NavTriNode* nEnd, Vector3 from, Vector3 to, NavigationPath& outPath)
{
	std::vector<Vector3> pts;
	vector<NavTriNode*> nodes;
	nodes.emplace_back(nEnd);
	pts.emplace_back(to);
	NavTriNode* node = nEnd->parent;
	while (node->parent != nullptr) {
		nodes.emplace_back(node);
		pts.emplace_back(node->centroid);
		node = node->parent;
	}
	nodes.emplace_back(node);
	std::reverse(pts.begin(), pts.end());
	std::reverse(nodes.begin(), nodes.end());

	if (nodes.size() == 2) {
		outPath.PushWaypoint(to);
		outPath.PushWaypoint(from);
	}

	vector<Vector2> portals;
	NavTriEdge edgeLast;
	NavTriEdge edgeCur;
	nodes[0]->GetEdgeFromNeighbour(nodes[1], edgeLast);
	for (int i = 1; i < (nodes.size()-1); ++i) {
		assert(nodes[i]->GetEdgeFromNeighbour(nodes[i+1], edgeCur));
		int idx = NavTriEdge::IdxOverlap(edgeLast, edgeCur);
		if (i == 0) {
			Vector3& tmp = allVerts[edgeLast.TheOtherPt(idx)];
			portals.emplace_back(Vector2(tmp.x, tmp.z));
		}
		Vector3& tmp = allVerts[idx];
		portals.emplace_back(Vector2(tmp.x, tmp.z));
		edgeLast = edgeCur;
	}
	//for (int i = 0; i < portals.size()-1; ++i) {
	//	Debug::DrawLine(Vector3(portals[i].x, 1, portals[i].y), Vector3(portals[i+1].x, 1, portals[i+1].y));
	//}
}

float triarea2(const Vector2& a, const Vector2& b, const Vector2& c)
{
	const float ax = b.x - a.x;
	const float ay = b.y - a.y;
	const float bx = c.x - a.x;
	const float by = c.y - a.y;
	return bx * ay - ax * by;
}

int stringPull(const vector<Vector2>& portals, vector<Vector2>& pts)
{
	int nportals = portals.size();
	int maxPts = 1000;

	// Find straight path.
	int npts = 0;
	// Init scan state
	Vector2 portalApex, portalLeft, portalRight;
	int apexIndex = 0, leftIndex = 0, rightIndex = 0;
	portalApex = portals[0];
	portalLeft = portals[0];
	portalRight = portals[1];

	// Add start point.
	pts.emplace_back(portalApex);
	npts++;

	for (int i = 1; i < nportals && npts < maxPts; ++i)
	{
		Vector2 left = portals[i * 2 + 0];
		Vector2 right = portals[i * 2 + 1];

		// Update right vertex.
		if (triarea2(portalApex, portalRight, right) <= 0.0f)
		{
			if (ApproxEqual(portalApex, portalRight) || triarea2(portalApex, portalLeft, right) > 0.0f)
			{
				// Tighten the funnel.
				portalRight = right;
				rightIndex = i;
			}
			else
			{
				// Right over left, insert left to path and restart scan from portal left point.
				pts[npts] = portalLeft;
				npts++;
				// Make current left the new apex.
				portalApex = portalLeft;
				apexIndex = leftIndex;
				// Reset portal
				portalLeft = portalApex;
				portalRight = portalApex;
				leftIndex = apexIndex;
				rightIndex = apexIndex;
				// Restart scan
				i = apexIndex;
				continue;
			}
		}

		// Update left vertex.
		if (triarea2(portalApex, portalLeft, left) >= 0.0f)
		{
			if (ApproxEqual(portalApex, portalLeft) || triarea2(portalApex, portalRight, left) < 0.0f)
			{
				// Tighten the funnel.
				portalLeft = left;
				leftIndex = i;
			}
			else
			{
				// Left over right, insert right to path and restart scan from portal right point.
				pts[npts] = portalRight;
				npts++;
				// Make current right the new apex.
				portalApex = portalRight;
				apexIndex = rightIndex;
				// Reset portal
				portalLeft = portalApex;
				portalRight = portalApex;
				leftIndex = apexIndex;
				rightIndex = apexIndex;
				// Restart scan
				i = apexIndex;
				continue;
			}
		}
	}
	// Append last point to path.
	if (npts < maxPts)
	{
		//pts[npts] = portals[(nportals - 1) * 4 + 0]);
		npts++;
	}

	return npts;
}

//int stringPull(const float* portals, int nportals,
//	float* pts, const int maxPts)
//{
//	// Find straight path.
//	int npts = 0;
//	// Init scan state
//	float portalApex[2], portalLeft[2], portalRight[2];
//	int apexIndex = 0, leftIndex = 0, rightIndex = 0;
//	vcpy(portalApex, &portals[0]);
//	vcpy(portalLeft, &portals[0]);
//	vcpy(portalRight, &portals[2]);
//
//	// Add start point.
//	vcpy(&pts[npts * 2], portalApex);
//	npts++;
//
//	for (int i = 1; i < nportals && npts < maxPts; ++i)
//	{
//		const float* left = &portals[i * 4 + 0];
//		const float* right = &portals[i * 4 + 2];
//
//		// Update right vertex.
//		if (triarea2(portalApex, portalRight, right) <= 0.0f)
//		{
//			if (vequal(portalApex, portalRight) || triarea2(portalApex, portalLeft, right) > 0.0f)
//			{
//				// Tighten the funnel.
//				vcpy(portalRight, right);
//				rightIndex = i;
//			}
//			else
//			{
//				// Right over left, insert left to path and restart scan from portal left point.
//				vcpy(&pts[npts * 2], portalLeft);
//				npts++;
//				// Make current left the new apex.
//				vcpy(portalApex, portalLeft);
//				apexIndex = leftIndex;
//				// Reset portal
//				vcpy(portalLeft, portalApex);
//				vcpy(portalRight, portalApex);
//				leftIndex = apexIndex;
//				rightIndex = apexIndex;
//				// Restart scan
//				i = apexIndex;
//				continue;
//			}
//		}
//
//		// Update left vertex.
//		if (triarea2(portalApex, portalLeft, left) >= 0.0f)
//		{
//			if (vequal(portalApex, portalLeft) || triarea2(portalApex, portalRight, left) < 0.0f)
//			{
//				// Tighten the funnel.
//				vcpy(portalLeft, left);
//				leftIndex = i;
//			}
//			else
//			{
//				// Left over right, insert right to path and restart scan from portal right point.
//				vcpy(&pts[npts * 2], portalRight);
//				npts++;
//				// Make current right the new apex.
//				vcpy(portalApex, portalRight);
//				apexIndex = rightIndex;
//				// Reset portal
//				vcpy(portalLeft, portalApex);
//				vcpy(portalRight, portalApex);
//				leftIndex = apexIndex;
//				rightIndex = apexIndex;
//				// Restart scan
//				i = apexIndex;
//				continue;
//			}
//		}
//	}
//	// Append last point to path.
//	if (npts < maxPts)
//	{
//		vcpy(&pts[npts * 2], &portals[(nportals - 1) * 4 + 0]);
//		npts++;
//	}
//
//	return npts;
//}

bool NCL::CSC8503::CmpNavTriNodePtr(const NavTriNode* a, const NavTriNode* b)
{
	return (a->f < b->f);
}
