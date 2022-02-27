#include <iostream>

#include "Vector4.h"
#include "Vector3.h"
#include "Matrix3.h"
#include "Matrix4.h"

using namespace std;
using namespace NCL::Maths;

void main() {
	/*float element[16] = {	1,2,3,4,
							2,3,4,5,
							4,5,6,7,
							2,5,4,5 };*/
	//float element[16] = {	1,2,4,2,
	//						2,3,5,5,
	//						3,4,6,4,
	//						4,5,12,5 };
	//Matrix4 mat(element);
	//Vector4 vec(30, 40, 50, 60);
	//Vector4 rslt = Matrix4::Solve(mat, vec);
	//cout << rslt << endl;


	float element[9] = {	1,2,4,
							2,3,5,
							3,4,12};
	Matrix3 mat(element);
	Vector3 vec(23, 33, 83);
	Vector3 rslt = Matrix3::Solve(mat, vec);
	cout << rslt << endl;
}