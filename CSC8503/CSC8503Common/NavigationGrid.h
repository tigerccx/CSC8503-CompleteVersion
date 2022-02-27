#pragma once
#include "NavigationMap.h"
#include <string>
#include <set>
#include <thread>

namespace NCL {
	namespace CSC8503 {
		struct GridNode {
			GridNode* parent;

			GridNode* connected[4];
			int		  costs[4];

			Vector3		position;

			float f;
			float g;

			int type;

			GridNode() {
				for (int i = 0; i < 4; ++i) {
					connected[i] = nullptr;
					costs[i] = 0;
				}
				f = 0;
				g = 0;
				type = 0;
				parent = nullptr;
			}
			~GridNode() {	}
		};

		inline bool CmpGridNodePtr(const GridNode* a, const GridNode* b);
		typedef std::set<GridNode*, decltype(CmpGridNodePtr)*> NavigationNodeOpenList;
		typedef std::set<GridNode*> NavigationNodeCloseList;

		class NavigationGrid : public NavigationMap	{
		public:
			NavigationGrid();
			NavigationGrid(const std::string&filename);
			~NavigationGrid();

			bool FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath) override;
				
		protected:
			bool		NodeInOpenList(GridNode* n, NavigationNodeOpenList& list) const;
			bool		NodeInCloseList(GridNode* n, NavigationNodeCloseList& list) const;
			GridNode*	RemoveBestNode(NavigationNodeOpenList& list) const;
			float		Heuristic(GridNode* hNode, GridNode* endNode) const;
			int nodeSize;
			int gridWidth;
			int gridHeight;

			GridNode* allNodes;
		};
	}
}

