#pragma once
#include "../CSC8503Common/Component.h"
#include "../../Common/Maths.h"
#include "../CSC8503Common/NavigationMesh.h"
#include <string>

namespace NCL {
    namespace CSC8503 {
        class NavigationPath;
        class NavigationMesh;
    }
}

using namespace NCL;
using namespace NCL::CSC8503;

class NavigationController :
    public Component
{
public:
    ~NavigationController();
    bool LoadNavigationMap(const std::string& path);
    NavMeshPathFindState FindPath(const Maths::Vector3& from, const Maths::Vector3& to, NavigationPath& outPath);
    Maths::Vector3 GetNearestVert(const Maths::Vector3& pt);

protected:
    NavigationMesh* navMesh;
};

