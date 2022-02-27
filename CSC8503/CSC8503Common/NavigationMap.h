#pragma once
#include "../../Common/Vector3.h"
#include "NavigationPath.h"
#include <vector>
#include <thread>

namespace NCL {
	using namespace NCL::Maths;
	namespace CSC8503 {
		struct NavigationMapPathfindingRequest {
			NavigationMapPathfindingRequest(const Vector3& from, const Vector3& to) :
				ptFrom(from), ptTo(to) { }
			const Vector3 ptFrom;
			const Vector3 ptTo;
			NavigationPath path;
			bool found = false;
		};

		class NavigationMap
		{
		public:
			NavigationMap() {}
			~NavigationMap() {}

			virtual void FindPathsMultiThread(std::vector<NavigationMapPathfindingRequest>& requests) {
				std::vector<std::thread> threads;
				for (int i = 0; i < requests.size(); ++i) {
					threads.emplace_back(std::move(std::thread(&NavigationMap::FindPathThread, this, std::ref(requests[i]))));
				}

				for (int i = 0; i < threads.size(); ++i) { //Join for now, maybe need to be more complicated(eg. callback and things)
					threads[i].join();
				}
			}
			
			virtual bool FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath) = 0;
		protected:
			void FindPathThread(NavigationMapPathfindingRequest& request) {
				request.found = FindPath(request.ptFrom, request.ptTo, request.path);
			}
		};
	}
}

