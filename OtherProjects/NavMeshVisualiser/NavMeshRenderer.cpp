#include "NavMeshRenderer.h"
#include "../../Common/Assets.h"
#include "../../Common/Camera.h"

#include "../../Common/Vector3.h"
#include "../../Common/Matrix4.h"
#include "../../Common/MeshAnimation.h"

#include "../../Common/Exceptions.h"

#include <fstream>
#include <iostream>
using namespace NCL;

NavMeshRenderer::NavMeshRenderer() : OGLRenderer(*Window::GetWindow())	{

}

NCL::NavMeshRenderer::NavMeshRenderer(const std::string filename):
	OGLRenderer(*Window::GetWindow())
{
	navMesh = new OGLMesh();

	std::ifstream mapFile(Assets::DATADIR + filename);

	if (!mapFile) {
		throw NotImplementedException();
	}

	int vCount = 0;
	int iCount = 0;

	mapFile >> vCount;
	mapFile >> iCount;

	vector<Vector3>			meshVerts;
	vector<Vector4>			meshColours;
	vector<unsigned int>	meshIndices;

	for (int i = 0; i < vCount; ++i) {
		Vector3 temp;
		mapFile >> temp.x;
		mapFile >> temp.y;
		mapFile >> temp.z;
		meshVerts.emplace_back(temp);
		meshColours.emplace_back(Vector4(0, 1, 0, 1));
	}

	for (int i = 0; i < iCount; ++i) {
		unsigned int temp = -1;
		mapFile >> temp;
		meshIndices.emplace_back(temp);
	}

	struct TriNeighbours {
		int indices[3];
	};

	int numTris = iCount / 3;	//the indices describe n / 3 triangles
	vector< TriNeighbours> allNeighbours;
	//Each of these triangles will be sharing edges with some other triangles
	//so it has a maximum of 3 'neighbours', desribed by an index into n / 3 tris
	//if its a -1, then the edge is along the edge of the map...
	for (int i = 0; i < numTris; ++i) {
		TriNeighbours neighbours;
		mapFile >> neighbours.indices[0];
		mapFile >> neighbours.indices[1];
		mapFile >> neighbours.indices[2];
		allNeighbours.emplace_back(neighbours);
	}

	navMesh->SetVertexPositions(meshVerts);
	navMesh->SetVertexColours(meshColours);
	navMesh->SetVertexIndices(meshIndices);

	navMesh->UploadToGPU();

	vector<Vector3> centroids;
	vector<Vector4> centreColours;
	vector<unsigned int> lines;

	for (int i = 0; i < iCount; i += 3) {
		Vector3 a = meshVerts[meshIndices[i + 0]];
		Vector3 b = meshVerts[meshIndices[i + 1]];
		Vector3 c = meshVerts[meshIndices[i + 2]];

		Vector3 middle = (a + b + c) / 3.0f;
		centroids.emplace_back(middle);

		centreColours.emplace_back(Vector4(1, 0, 0, 1));
	}

	for (int i = 0; i < numTris; ++i) {
		TriNeighbours& n = allNeighbours[i];
		for (int j = 0; j < 3; ++j) {
			if (n.indices[j] != -1) {
				TriNeighbours& nj = allNeighbours[n.indices[j]];

				lines.emplace_back(i);
				lines.emplace_back(n.indices[j]);
			}
		}
	}
	centroidMesh = new OGLMesh();
	centroidMesh->SetVertexPositions(centroids);
	centroidMesh->SetVertexColours(centreColours);
	centroidMesh->SetVertexIndices(lines);
	centroidMesh->SetPrimitiveType(NCL::GeometryPrimitive::Lines);
	centroidMesh->UploadToGPU();

	shader = new OGLShader("DebugVert.glsl", "DebugFrag.glsl");

	camera = new Camera();
	camera->SetNearPlane(1.0f);
	camera->SetFarPlane(1000.0f);
	camera->SetPosition(Vector3(0, 3, 10));
}

NavMeshRenderer::~NavMeshRenderer() {
	delete navMesh;
	delete camera;
}

void NavMeshRenderer::Update(float dt) {
	camera->UpdateCamera(dt);
}

Matrix4 NavMeshRenderer::SetupDebugLineMatrix()	const {
	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = camera->BuildViewMatrix();
	Matrix4 projMatrix = camera->BuildProjectionMatrix(screenAspect);

	return projMatrix * viewMatrix;
}

Matrix4 NavMeshRenderer::SetupDebugStringMatrix()	const {
	return Matrix4::Orthographic(-1, 1.0f, 100, 0, 0, 100);
}

void NavMeshRenderer::RenderFrame() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	BindShader(shader);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	float screenAspect = (float)currentWidth / (float)currentHeight;

	Matrix4 viewMatrix = camera->BuildViewMatrix();
	Matrix4 projMatrix = camera->BuildProjectionMatrix(screenAspect);

	Matrix4 vp = projMatrix * viewMatrix;

	int vpLocation		= glGetUniformLocation(shader->GetProgramID(), "viewProjMatrix");
	glUniformMatrix4fv(vpLocation, 1, false, (float*)&vp);

	BindMesh(navMesh);
	DrawBoundMesh();

	//BindMesh(centroidMesh);
	//DrawBoundMesh();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}