#pragma once
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include <string>

namespace NCL {
	class Camera;
	class MeshAnimation;

	class NavMeshRenderer : public OGLRenderer
	{
	public:
		NavMeshRenderer();
		NavMeshRenderer(const std::string filename);
		virtual ~NavMeshRenderer();
		void Update(float dt) override;

		inline Camera* GetCamera() const {
			return camera;
		}

	protected:
		void RenderFrame()	override;

		Matrix4 SetupDebugLineMatrix()	const override;
		Matrix4 SetupDebugStringMatrix() const override;

		OGLMesh*	navMesh;
		OGLMesh*	centroidMesh;
		Camera*		camera;
		OGLShader*  shader;
	};
}

