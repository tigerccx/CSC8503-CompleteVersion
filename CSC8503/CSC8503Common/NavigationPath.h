#pragma once
#include "../../Common/Vector3.h"
#include <vector>
#include <algorithm>

namespace NCL {
	using namespace NCL::Maths;
	namespace CSC8503 {
		class NavigationPath		{
		public:
			NavigationPath() {}
			~NavigationPath() {}

			void	Clear() {
				waypoints.clear();
			}
			inline int GetSize() { return waypoints.size(); }
			void	PushWaypoint(const Vector3& wp) {
				waypoints.emplace_back(wp);
			}
			bool	PopWaypoint(Vector3& waypoint) {
				if (waypoints.empty()) {
					return false;
				}
				waypoint = waypoints.back();
				waypoints.pop_back();
				return true;
			}
			void Flip() {
				std::reverse(waypoints.begin(), waypoints.end());
			}

			std::vector <Vector3> waypoints;

		protected:

			
		};
	}
}

