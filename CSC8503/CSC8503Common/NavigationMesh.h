#pragma once
#include "NavigationMap.h"
#include "../../Common/Plane.h"
#include <string>
#include <vector>
#include <set>
#include <stack>
#include "../../Common/Vector3.h"

namespace NCL {
	namespace CSC8503 {
		/*struct NavTri {
			Plane   triPlane;
			Vector3 centroid;
			float	area;
			NavTri* neighbours[3];

			int indices[3];

			NavTri() {
				area = 0.0f;
				neighbours[0] = nullptr;
				neighbours[1] = nullptr;
				neighbours[2] = nullptr;

				indices[0] = -1;
				indices[1] = -1;
				indices[2] = -1;
			}
		};*/

		//typedef std::pair<int, int> NavTriEdge;

		struct NavTriEdge
		{
			NavTriEdge() {}
			NavTriEdge(int idx0, int idx1) :
				i0(idx0), i1(idx1) {}
			int i0 = -1;
			int i1 = -1;

			inline bool HasPt(int idx) const {
				return i0 == idx || i1 == idx;
			}
			inline int TheOtherPt(int idx) const {
				if (i0 == idx) return i1;
				if (i1 == idx) return i0;
				return -1;
			}

			static inline int IdxOverlap(const NavTriEdge& e0, const NavTriEdge& e1) {
				if (e0.i0 == e1.i0 || e0.i0 == e1.i1) return e0.i0;
				if (e0.i1 == e1.i0 || e0.i1 == e1.i1) return e0.i1;
				return -1;
			}
			
		};

		struct NavTriNode {
			Plane   triPlane;
			Vector3 centroid;
			float	area;
			int indices[3];
			NavTriNode* neighbours[3];
			NavTriNode* parent;
			float costs[3];
			float f;
			float g;
			
			

			NavTriNode() {
				area = 0.0f;
				neighbours[0] = nullptr;
				neighbours[1] = nullptr;
				neighbours[2] = nullptr;

				indices[0] = -1;
				indices[1] = -1;
				indices[2] = -1;

				costs[0] = 0;
				costs[1] = 0;
				costs[2] = 0;

				f = 0;
				g = 0;
			}

			bool GetEdgeFromNeighbour(NavTriNode* neighbour, NavTriEdge& edge) {
				int idxNeighbour = -1;
				for (int i = 0; i < 3; ++i) {
					if (neighbours[i] == neighbour) {
						idxNeighbour = i;
						break;
					}
				}
				if (idxNeighbour < 0)
					return false;
				edge.i0 = indices[idxNeighbour];
				edge.i1 = indices[(idxNeighbour + 1) % 3];
				return true;
			}

			bool GetEdgeFromNeighbour(int idxNeighbour, NavTriEdge& edge) {
				if (idxNeighbour >= 3)
					return false;
				edge.i0 = indices[idxNeighbour];
				edge.i1 = indices[(idxNeighbour + 1) % 3];
				return true;
			}
		};

		inline bool CmpNavTriNodePtr(const NavTriNode* a, const NavTriNode* b);

		typedef std::set<NavTriNode*, decltype(CmpNavTriNodePtr)*> NavTriNodeOpenList;
		typedef std::set<NavTriNode*> NavTriNodeCloseList;

		enum NavMeshPathFindState {
			NavMeshPathFindState_Success,
			NavMeshPathFindState_FromNotOnGrid,
			NavMeshPathFindState_ToNotOnGrid,
			NavMeshPathFindState_UnableToFindPath
		};

		class NavigationMesh : public NavigationMap	{
		public:
			NavigationMesh();
			NavigationMesh(const std::string&filename);
			~NavigationMesh();

			bool enableSmoothing = false;
			float smoothToBoundaryDistance = 0.5f;

			bool FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath) override;
			NavMeshPathFindState FindPath1(const Vector3& from, const Vector3& to, NavigationPath& outPath);

			Vector3 GetNearestVertOnMesh(const Vector3& point);
		
		protected:
			NavTriNode* GetTriForPosition(const Vector3& pos);

			bool		NodeInOpenList(NavTriNode* n, NavTriNodeOpenList& list) const;
			bool		NodeInCloseList(NavTriNode* n, NavTriNodeCloseList& list) const;
			NavTriNode* RemoveBestNode(NavTriNodeOpenList& list) const;
			float		Heuristic(NavTriNode* hNode, NavTriNode* endNode) const;

			void		PathwayEdgeCentSmoothing(NavTriNode* nEnd, Vector3 from, Vector3 to, NavigationPath& outPath);
			void		PathwaySGDSmoothing(NavTriNode* nEnd, Vector3 from, Vector3 to, NavigationPath& outPath);
			void		PathwayMyWierdSmoothing(NavTriNode* nEnd, Vector3 from, Vector3 to, NavigationPath& outPath);
			void		PathwayFunnelSmoothing(NavTriNode* nEnd, Vector3 from, Vector3 to, NavigationPath& outPath);

			std::vector<NavTriNode>		allTris;
			std::vector<Vector3>	allVerts;
		};
	}
}