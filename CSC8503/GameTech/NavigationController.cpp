#include "NavigationController.h"
#include "../CSC8503Common/NavigationMesh.h"
#include "../CSC8503Common/NavigationPath.h"
#include <cassert>

NavigationController::~NavigationController()
{
	delete navMesh;
}

bool NavigationController::LoadNavigationMap(const std::string& path)
{
	navMesh = new NavigationMesh(path);
	navMesh->enableSmoothing = true;
	return true;
}

NavMeshPathFindState NavigationController::FindPath(const Maths::Vector3& from, const Maths::Vector3& to, NavigationPath& outPath)
{
	assert(navMesh);
	return navMesh->FindPath1(from, to, outPath);
}

Maths::Vector3 NavigationController::GetNearestVert(const Maths::Vector3& pt)
{
	return navMesh->GetNearestVertOnMesh(pt);
}
