// ◦ Xyz ◦

#include "Glider.h"
#include "GliderParams.h"
#include <Common/Help.h>
#include <Callback/Callback.h>
#include <Draw/Camera/Camera.h>

#if _DEBUG
#include <Draw2/Draw2.h>
#include <Draw2/Shader/ShaderLine.h>
#endif

using namespace glider;

void Glider::EnableControl(bool enable)
{
	if (!_callbackPtr && enable) {
		_callbackPtr = std::make_shared<Engine::Callback>(Engine::CallbackType::PINCH_KEY, [this](const Engine::CallbackEventPtr& callbackEventPtr) {
			float kForce = 1.0;

			if (Engine::Callback::pressKey(Engine::VirtualKey::CONTROL)) {
				kForce = 0.5f;
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
			if (Engine::Callback::pressKey(Engine::VirtualKey::A)) {
				Move(MoveDirect::LEFT, kForce);
			}
			if (Engine::Callback::pressKey(Engine::VirtualKey::D)) {
				Move(MoveDirect::RIGHT, kForce);
			}

#if _DEBUG
			if (Engine::Callback::pressKey(Engine::VirtualKey::F12)) {
				ResetPosition();
			}
#endif
		});

#if _DEBUG
		_callbackPtr->debugName = "Glider";
#endif
	}

	if (!enable && _callbackPtr) {
		_callbackPtr.reset();
	}
}

void Glider::Move(const MoveDirect direct, const float kForce)
{
	glm::vec4 cameraDirectV4 = getMatrix()* glm::vec4(0.f, -1.f, 0.f, 0.f);
	glm::vec3 cameraDirect = glm::normalize(glm::vec3(cameraDirectV4.x, cameraDirectV4.y, 0.f));

	const float force = GetParams().force;
	glm::vec3 vectorForce = { 0.f, 0.f, 0.f };

	switch (direct)
	{
	case MoveDirect::FORVARD:
		vectorForce += cameraDirect * force * kForce;
		_force += vectorForce;
		break;

	case MoveDirect::BACK:
		vectorForce -= cameraDirect * force * kForce;
		_force += vectorForce;
		break;

	case MoveDirect::LEFT:
		vectorForce.x -= (cameraDirect.y * force * kForce);
		vectorForce.y += (cameraDirect.x * force * kForce);
		_force += vectorForce;
		break;

	case MoveDirect::RIGHT:
		vectorForce.x += (cameraDirect.y * force * kForce);
		vectorForce.y -= (cameraDirect.x * force * kForce);
		_force += vectorForce;
		break;
	}
}

const Params& Glider::GetParams()
{
	if (!paramsPtr) {
		paramsPtr = Params::GetDefault();
	}
	return *paramsPtr;
}

void Glider::action()
{
	Stabilization();

	addTorque(_torqueForce);
	addForce(_force);
	ResetForce();

#if _DEBUG
	DrawDebug();
#endif
}

void Glider::Stabilization()
{
	const float height = GetHeight();
	const auto& params = GetParams();

	// ...
	if (height < GetParams().height) {
		const float velocityZ = GetLinearVelocity().z;
		const float delta = std::max(0.f, GetParams().height - height);
		const float forceZ = delta * params.liftingForce - velocityZ * params.liftingDamping;
		_force.z += forceZ;
	}

	glm::mat4x4 mat = getMatrix();

	//...
	const glm::vec4 directV4 = glm::vec4(0.f, -1.f, 0.f, 0.f);
	const glm::vec3 direct = glm::normalize(glm::vec3(directV4.x, directV4.y, 0.f));

	const glm::vec3 camDirect = Camera::GetLink().Direct();
	const glm::vec3 camdirect = glm::normalize(glm::vec3(camDirect.x, camDirect.y, 0.f));

	const float dotProduct = glm::dot(direct, camdirect);
	const glm::vec3 crossProduct = glm::cross(direct, camdirect);

	float angleRadians = glm::atan(glm::length(crossProduct), dotProduct);
	if (crossProduct.z < 0) {
		angleRadians = -angleRadians;
	}

	mat = glm::rotate(mat, -angleRadians, { 0.f, 0.f, 1.f });
	
	//...
	const glm::quat quat = glm::quat_cast(mat);
	const glm::vec3 axis = glm::eulerAngles(quat);
	const float angle = glm::angle(quat);
	const glm::vec3 correctiveTorque = axis * (-angle * params.stabilizationForce);
	const glm::vec3 angularVelocity = GetAngularVelocity();
	const glm::vec3 dampingTorque = angularVelocity * (-params.stabilizationDamping);

	_torqueForce += correctiveTorque + dampingTorque;
}

float Glider::GetHeight()
{
	constexpr glm::vec3 directDown(0.f, 0.f, -1.f);
	glm::vec3 pos = getPos();
	pos.z += 1.f;
	const glm::vec3 hitPos = Engine::Physics::Raycast(pos, directDown);
	const float height = pos.z - hitPos.z;
	return height;
}

void Glider::ResetForce()
{
	_force.x = 0.f;
	_force.y = 0.f;
	_force.z = 0.f;
	_torqueForce.x = 0.f;
	_torqueForce.y = 0.f;
	_torqueForce.z = 0.f;
}

#if _DEBUG
void Glider::DrawDebug()
{
	Draw2::AddFunction([this]() mutable {
		{
			ShaderLinePM::Instance().Use();

			glm::mat4x4 matSpace(1.f);
			matSpace[3][0] = getMatrix()[3][0];
			matSpace[3][1] = getMatrix()[3][1];
			matSpace[3][2] = getMatrix()[3][2];
			Draw2::SetModelMatrixClass<ShaderLinePM>(matSpace);

			// Space
			{
				// X
				{
					float red[] = { 1.f, 0.f, 0.f, 0.125f };
					Draw2::SetColorClass<ShaderLinePM>(red);
					glm::vec4 direct(1.f, 0.f, 0.f, 0.f);
					direct = matSpace * direct;
					direct *= 100.f;
					const float vertices[] = { 0.f, 0.f, 0.f, direct.x, direct.y, direct.z };
					const unsigned int count = 2;
					Draw2::drawLines(vertices, count);
				}

				// Y
				{
					float green[] = { 0.f, 1.f, 0.f, 0.125f };
					Draw2::SetColorClass<ShaderLinePM>(green);
					glm::vec4 direct(0.f, 1.f, 0.f, 0.f);
					direct = matSpace * direct;
					direct *= 100.f;
					const float vertices[] = { 0.f, 0.f, 0.f, direct.x, direct.y, direct.z };
					const unsigned int count = 2;
					Draw2::drawLines(vertices, count);
				}

				// Z
				{	
					float blue[] = { 0.f, 0.f, 1.f, 0.125f };
					Draw2::SetColorClass<ShaderLinePM>(blue);
					glm::vec4 direct(0.f, 0.f, 1.f, 0.f);
					direct = matSpace * direct;
					direct *= 100.f;
					const float vertices[] = { 0.f, 0.f, 0.f, direct.x, direct.y, direct.z };
					const unsigned int count = 2;
					Draw2::drawLines(vertices, count);
				}
			}

			// Glider
			{
				// X
				{
					float red[] = { 1.f, 0.f, 0.f, 0.25f };
					Draw2::SetColorClass<ShaderLinePM>(red);
					glm::vec4 direct(1.f, 0.f, 0.f, 0.f);
					direct = getMatrix() * direct;
					direct *= 100.f;
					const float vertices[] = { 0.f, 0.f, 0.f, direct.x, direct.y, direct.z };
					const unsigned int count = 2;
					Draw2::drawLines(vertices, count);
				}

				// Y
				{
					float green[] = { 0.f, 1.f, 0.f, 0.25f };
					Draw2::SetColorClass<ShaderLinePM>(green);
					glm::vec4 direct(0.f, 1.f, 0.f, 0.f);
					direct = getMatrix() * direct;
					direct *= 100.f;
					const float vertices[] = { 0.f, 0.f, 0.f, direct.x, direct.y, direct.z };
					const unsigned int count = 2;
					Draw2::drawLines(vertices, count);
				}

				// Z
				{
					float blue[] = { 0.f, 0.f, 1.f, 0.25f };
					Draw2::SetColorClass<ShaderLinePM>(blue);
					glm::vec4 direct(0.f, 0.f, 1.f, 0.f);
					direct = getMatrix() * direct;
					direct *= 100.f;
					const float vertices[] = { 0.f, 0.f, 0.f, direct.x, direct.y, direct.z };
					const unsigned int count = 2;
					Draw2::drawLines(vertices, count);
				}
			}
		}

		return false;
	});
}

void Glider::ResetPosition()
{
	SetLinearVelocity({ 0.f, 0.f , 0.f });
	SetAngularVelocity({ 0.f, 0.f , 0.f });
	setActorPos({ 0.f, 0.f , 75.f });
}
#endif