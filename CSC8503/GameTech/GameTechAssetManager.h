#pragma once
#include "AssetManager.h"

using namespace NCL;
using namespace CSC8503;

class GameTechAssetManager:
	public AssetManager
{
public:
	void LoadAllAssets() override;
};