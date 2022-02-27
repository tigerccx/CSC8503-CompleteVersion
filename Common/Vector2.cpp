/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
using namespace NCL;
using namespace Maths;

Vector2::Vector2(const Vector3& v3) : x(v3.x), y(v3.y){
}

Vector2::Vector2(const Vector4& v4) : x(v4.x), y(v4.y) {
}

#include <sstream>
#include <string>
Vector2 NCL::Maths::Vector2::FromUnityString(const std::string str)
{
    std::stringstream ss;
    Vector3 ret;
    ss << str;
    ss >> ret.x;
    ss >> ret.y;
    return ret;
}