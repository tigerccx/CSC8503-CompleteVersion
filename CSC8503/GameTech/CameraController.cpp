#include "CameraController.h"
#include "../../Common/Window.h"

void CameraController::Update(float dt)
{
	if (camera) {
		//Update the mouse by how much
		camera->pitch -= (Window::GetMouse()->GetRelativePosition().y);
		camera->yaw -= (Window::GetMouse()->GetRelativePosition().x);

		//Bounds check the pitch, to be between straight up and straight down ;)
		camera->pitch = std::min(camera->pitch, 90.0f);
		camera->pitch = std::max(camera->pitch, -90.0f);

		if (camera->yaw < 0) {
			camera->yaw += 360.0f;
		}
		if (camera->yaw > 360.0f) {
			camera->yaw -= 360.0f;
		}

		float frameSpeed = 100 * dt;

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
			camera->position += Matrix4::Rotation(camera->yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * frameSpeed;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
			camera->position -= Matrix4::Rotation(camera->yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * frameSpeed;
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
			camera->position += Matrix4::Rotation(camera->yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * frameSpeed;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
			camera->position -= Matrix4::Rotation(camera->yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * frameSpeed;
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SHIFT)) {
			camera->position.y += frameSpeed;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE)) {
			camera->position.y -= frameSpeed;
		}
	}
}
