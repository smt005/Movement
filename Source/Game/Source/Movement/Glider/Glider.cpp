// ◦ Xyz ◦

#include "Glider.h"
#include "../../Engine/Source/Callback/Callback.h"
#include <Draw/Camera/Camera.h>

using namespace glider;

void Glider::EnableControl(bool enable)
{
	if (!_callbackPtr && enable) {
		_callbackPtr = std::make_shared<Engine::Callback>(Engine::CallbackType::PINCH_KEY, [this](const Engine::CallbackEventPtr& callbackEventPtr) {
			float kForce = 1.0;

			if (Engine::Callback::pressKey(Engine::VirtualKey::CONTROL)) {
				kForce = 0.5f;
				return;
			}
			else if (Engine::Callback::pressKey(Engine::VirtualKey::SHIFT)) {
				kForce = 2.f;
			}

			if (Engine::Callback::pressKey(Engine::VirtualKey::W)) {
				Move(MoveDirect::FORVARD, kForce);
			}
			if (Engine::Callback::pressKey(Engine::VirtualKey::S)) {
				Move(MoveDirect::BACK, kForce);
			}
			if (Engine::Callback::pressKey(Engine::VirtualKey::D)) {
				Move(MoveDirect::RIGHT, kForce);
			}
			if (Engine::Callback::pressKey(Engine::VirtualKey::A)) {
				Move(MoveDirect::LEFT, kForce);
			}
			if (Engine::Callback::pressKey(Engine::VirtualKey::R)) {
				Move(MoveDirect::TOP, kForce);
			}
			if (Engine::Callback::pressKey(Engine::VirtualKey::F)) {
				Move(MoveDirect::DOWN, kForce);
			}
		});
	}

	if (!enable && _callbackPtr) {
		_callbackPtr.reset();
	}
}

void Glider::Move(const MoveDirect direct, const float kForce) {
	glm::vec3 cameraDirect = Camera::GetLink().Direct();
	cameraDirect.z = 0.f;
	cameraDirect = glm::normalize(cameraDirect);

	float force = GetParams().force;
	glm::vec3 vectorForce = { 0.f, 0.f, 0.f };

	switch (direct)
	{
	case MoveDirect::FORVARD:
		vectorForce += cameraDirect * force * kForce;
		break;

	case MoveDirect::BACK:
		vectorForce -= cameraDirect * force * kForce;
		break;

	case MoveDirect::LEFT:
		vectorForce.x -= (cameraDirect.y * force * kForce);
		vectorForce.y += (cameraDirect.x * force * kForce);
		break;

	case MoveDirect::RIGHT:
		vectorForce.x += (cameraDirect.y * force * kForce);
		vectorForce.y -= (cameraDirect.x * force * kForce);
		break;

	case MoveDirect::TOP:
		vectorForce.z += abs(force * kForce);
		break;

	case MoveDirect::DOWN:
		vectorForce.z -= abs(force * kForce);
		break;

	case MoveDirect::FORVARD_HORIZONT:
		vectorForce.x += (cameraDirect.x * force * kForce);
		vectorForce.y += (cameraDirect.y * force * kForce);
		break;

	case MoveDirect::BACK_HORIZONT:
		vectorForce.x -= (cameraDirect.x * force * kForce);
		vectorForce.y -= (cameraDirect.y * force * kForce);
		break;
	}

	this->addForce(vectorForce);
}

const Params& Glider::GetParams()
{
	if (!paramsPtr) {
		paramsPtr = Params::GetDefault();
	}
	return *paramsPtr;
}
