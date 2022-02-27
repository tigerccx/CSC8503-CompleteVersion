#include "CameraController1.h"
#include "../../Common/Window.h"
#include "CameraViewSelector.h"

void CameraController1::Update(float dt)
{
	if (camera) {
		if (Window::GetMouse()->ButtonHeld(MouseButtons::RIGHT)) { //Free movement mode
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
			if (selector) {
				selector->camera = nullptr;
				selector->Deactivate();
			}

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
		}
		else {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
			if (selector) {
				selector->camera = camera;
				selector->Activate();
			}
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

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::Q)) {
			camera->position.y += frameSpeed;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::E)) {
			camera->position.y -= frameSpeed;
		}
	}
}
