#include "AssetManager.h"

NCL::CSC8503::AssetManager::AssetManager()
{
	
}

NCL::CSC8503::AssetManager::~AssetManager()
{
	CleanupAllAssets();
}

void NCL::CSC8503::AssetManager::ReloadAllAssets()
{
	CleanupAllAssets();
	LoadAllAssets();
}

void NCL::CSC8503::AssetManager::CleanupAllAssets()
{
	for (auto& itr : meshes) {
		if (itr.second) {
			delete itr.second;
		}
	}

	for (auto& itr : textures) {
		if (itr.second) {
			delete itr.second;
		}
	}

	for (auto& itr : shaders) {
		if (itr.second) {
			delete itr.second;
		}
	}
}
