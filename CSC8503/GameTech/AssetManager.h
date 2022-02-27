#pragma once
#include <map>
#include <string>

#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"

namespace NCL {
	using namespace Rendering;
	namespace CSC8503 {
		class AssetManager
		{
		public:
			AssetManager();
			~AssetManager();

			void ReloadAllAssets();
			void CleanupAllAssets();

			inline OGLMesh* GetMesh(const std::string& name) const { auto itr = meshes.find(name); if (itr != meshes.end()) return itr->second; return nullptr; }
			inline OGLTexture* GetTexture(const std::string& name) const { auto itr = textures.find(name); if (itr != textures.end()) return itr->second; return nullptr; }
			inline OGLShader* GetShader(const std::string& name) const { auto itr = shaders.find(name); if (itr != shaders.end()) return itr->second; return nullptr; }

		protected:
			std::map<std::string, OGLMesh*> meshes;
			std::map<std::string, OGLTexture*> textures;
			std::map<std::string, OGLShader*> shaders;

			virtual void LoadAllAssets() = 0;
		};
	}
}


