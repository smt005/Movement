// ◦ Xyz ◦

#include "Glider.h"
#include "GliderParams.h"
#include <Common/Help.h>
#include "../../Engine/Source/Callback/Callback.h"

#if _DEBUG
#include <Draw/Camera/CameraControlOutside.h>
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
			if (Engine::Callback::pressKey(Engine::VirtualKey::F1)) {
				ResetPosition();
			}
#endif
		});
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
	// Высота
	const float height = GetHeight();
	const auto& params = GetParams();

	// ВЫсота
	if (height < GetParams().height)
	{
		float velocityZ = GetLinearVelocity().z;
		float delta = std::max(0.f, GetParams().height - height);
		volatile static float liftingForce = 1.f;
		volatile static float damping = 1.0f;
		float forceZ = delta * params.liftingForce - velocityZ * params.liftingDamping;
		glm::vec3 vector = { 0.f, 0.f, forceZ };
		_force += vector;
	}

	glm::mat4x4  mat = getMatrix();

	float rotateZ = 0.f;
	{
		glm::vec4 _direct4_ = glm::vec4(0.f, -1.f, 0.f, 0.f);
		glm::vec3 _direct3_ = glm::normalize(glm::vec3(_direct4_.x, _direct4_.y, 0.f));

		auto camDirect = Camera::GetLink().Direct();
		glm::vec3 _camdirect3_ = glm::normalize(glm::vec3(camDirect.x, camDirect.y, 0.f));

		//...
		float dotProduct = glm::dot(_direct3_, _camdirect3_);
		glm::vec3 crossProduct = glm::cross(_direct3_, _camdirect3_);

		// Угол в радианах
		float angleRadians = glm::atan(glm::length(crossProduct), dotProduct);
		if (crossProduct.z < 0) {
			angleRadians = -angleRadians;
		}

		rotateZ = -angleRadians;
	}

	mat = glm::rotate(mat, rotateZ, { 0.f, 0.f, 1.f });


	glm::quat q = glm::quat_cast(mat);

	glm::vec3 axis = glm::eulerAngles(q);

	float angle = glm::angle(q);
	static float angleMax = 0.f;
	angleMax = std::max(angleMax, angle);
	help::log(std::to_string(rotateZ) + " - "s + std::to_string(angle) + " - " + std::to_string(angleMax));

	static float damping = 1.f;        // Демпфирование угловой скорости

	static float torqueStrength = 10.0f;// *std::max(0.f, (4.7f - angle)); // Сила возвращения к целевой ориентации
	//static float torqueStrength = 1.f; // Сила возвращения к целевой ориентации

	// Рассчет корректирующего торка
	glm::vec3 correctiveTorque = axis * (-angle * torqueStrength);

	// Учет угловой скорости (демпфирование)
	glm::vec3 angularVelocity = GetAngularVelocity();
	glm::vec3 dampingTorque = angularVelocity * (-damping);

	// Применение суммарного торка
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