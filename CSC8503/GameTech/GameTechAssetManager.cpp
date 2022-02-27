#include "GameTechAssetManager.h"
#include "../../Common/TextureLoader.h"

void GameTechAssetManager::LoadAllAssets()
{
	auto loadFunc = [](const string& name, OGLMesh** into) {
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("cube.msh", &meshes["cube"]);
	loadFunc("sphere.msh", &meshes["sphere"]);
	loadFunc("Male1.msh", &meshes["charA"]);
	loadFunc("courier.msh", &meshes["charB"]);
	loadFunc("security.msh", &meshes["charC"]);
	loadFunc("coin.msh", &meshes["coin"]);
	loadFunc("capsule.msh", &meshes["capsule"]);

	textures["basic"] = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");

	shaders["basic"] = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");
}
