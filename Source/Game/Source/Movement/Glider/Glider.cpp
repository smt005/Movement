// ◦ Xyz ◦

#include "Glider.h"
#include <algorithm>
#include <functional>
#include <math.h>
#include "../../Engine/Source/Callback/Callback.h"
#include <Draw/Camera/Camera.h>
#include "Common/Help.h"
#include "Draw2/Draw2.h"
#include <Draw2/Shader/ShaderLine.h>

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

			//...
			if (Engine::Callback::pressKey(Engine::VirtualKey::I)) {
				_torqueForce.x += 0.01f;
			}
			if (Engine::Callback::pressKey(Engine::VirtualKey::K)) {
				_torqueForce.x -= 0.01f;
			}
			if (Engine::Callback::pressKey(Engine::VirtualKey::J)) {
				_torqueForce.y -= 0.01f;
			}
			if (Engine::Callback::pressKey(Engine::VirtualKey::L)) {
				_torqueForce.y += 0.01f;
			}

			// Height
			if (Engine::Callback::pressKey(Engine::VirtualKey::Y)) {
				paramsPtr->height += 10;
			}
			if (Engine::Callback::pressKey(Engine::VirtualKey::H)) {
				paramsPtr->height -= 10;
			}

			// Type
			if (Engine::Callback::pressKey(Engine::VirtualKey::CONTROL)) {
				if (Engine::Callback::pressKey(Engine::VirtualKey::VK_0)) {
					_torqueForceType = Engine::Physics::Force::ACCELERATION;
				}
				else if (Engine::Callback::pressKey(Engine::VirtualKey::VK_2)) {
					_torqueForceType = Engine::Physics::Force::FORCE;
				}
				else if (Engine::Callback::pressKey(Engine::VirtualKey::VK_3)) {
					_torqueForceType = Engine::Physics::Force::IMPULSE;
				}
				else if (Engine::Callback::pressKey(Engine::VirtualKey::VK_4)) {
					_torqueForceType = Engine::Physics::Force::VELOCITY_CHANGE;
				}
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

float limitRange(float value, float min = 0.f, int max = 1.f) {
	if (value < min) {
		return min;  // Если значение меньше минимального, возвращаем минимальное
	}
	if (value > max) {
		return max;  // Если значение больше максимального, возвращаем максимальное
	}
	return value;  // Если значение в пределах диапазона, возвращаем его без изменений
}

void Glider::action()
{
	// Высота
	if (GetHeight() < GetParams().height) {
		glm::vec3 velocity = GetLinearVelocity();
		velocity.z = 0.f;
		SetLinearVelocity(velocity);

		float value = GetParams().height - GetHeight();
		value /= GetParams().height;
		value = limitRange(value);

		glm::vec3 vector{ 0.f, 0.f, value };
		addForce(vector, Engine::Physics::Force::VELOCITY_CHANGE);
	}

	// Стабилизация
	{
		auto mat = this->getMatrix();
		glm::vec4 direct(0.f, 0.f, 1.f, 0.f);
		direct = mat * direct;
		glm::vec4 directDebug = direct;
		directDebug.z = 0.f;
		volatile static int _ver0_ = 1;
		volatile static int _ver1_ = 1;

		if (_ver0_ == 0) {
			static float factor = 0.1f;
			direct.y = direct.y * factor;
			direct.x = direct.x * factor;
		}
		else if (_ver0_ == 1) {
			static float factor = 1.91f;
			{
				if (direct.y != 0.f) {
					float sign = direct.y / std::abs(direct.y);
					direct.y = direct.y * direct.y * sign * factor;
				}
			}
			{
				if (direct.x != 0.f) {
					float sign = direct.x / std::abs(direct.x);
					direct.x = direct.x * direct.x * sign * factor;
				}
			}
		}

		direct.z = 0.f;

		glm::vec3 forceAngularVelocity(direct.y, -direct.x, 0.f);
		const glm::vec3 angularVelocity = GetAngularVelocity();

		if (_ver1_ == 1) {
			// X
			{
				float sign0 = false;
				float sign1 = false;
				if (direct.x != 0) {
					sign0 = direct.x / std::abs(direct.x);
				}
				if (angularVelocity.y != 0) {
					sign1 = !angularVelocity.y / std::abs(angularVelocity.y);
				}
				if ((sign0 > 0 && sign1 > 0) || (sign0 < 0 && sign1 < 0)) {
					forceAngularVelocity.x = 0.f;
				}
			}
			// Y
			{
				float sign0 = false;
				float sign1 = false;
				if (direct.y != 0) {
					sign0 = direct.y / std::abs(direct.y);
				}
				if (angularVelocity.x != 0) {
					sign1 = !angularVelocity.x / std::abs(angularVelocity.x);
				}
				if ((sign0 > 0 && sign1 > 0) || (sign0 < 0 && sign1 < 0)) {
					forceAngularVelocity.y = 0.f;
				}
			}
		}

		float len = glm::length(forceAngularVelocity);
		static float minValue = 0.00001f;
		if (len > minValue) {
			addTorque(forceAngularVelocity, _torqueForceType);
		}

		if (length(directDebug) < 0.001f) {
			this->SetAngularVelocity({0.f, 0.f, 0.f});
		}

		glm::vec3 forceAngularVelocityDebug;
		forceAngularVelocityDebug.x = forceAngularVelocity.y;
		forceAngularVelocityDebug.y = -forceAngularVelocity.x;
		forceAngularVelocityDebug.z = 0.f;
		//cout << "direct: [" << directDebug.x << ", " << directDebug.y << ", " << directDebug.z << "] vel: [" << angularVelocity.x << ", " << angularVelocity.y << ", " << angularVelocity.z << "] force: [" << forceAngularVelocityDebug.x << ", " << forceAngularVelocityDebug.y << ", " << forceAngularVelocityDebug.z << "]\n";

		DrawDebug(forceAngularVelocity);

		// Debug
		{
			this->addTorque(_torqueForce, Engine::Physics::Force::VELOCITY_CHANGE);
			_torqueForce.x = 0.f;
			_torqueForce.y = 0.f;
		}
	}
}

void Glider::Update()
{
	_velocity = this->GetLinearVelocity();
	_angularVelocity = this->GetAngularVelocity();
	_vectorAngularVelocity.x = _angularVelocity.y;
	_vectorAngularVelocity.y = -_angularVelocity.x;
	_vectorAngularVelocity.z = 0.f;

	cout << "v: [" << _velocity.x << ", " << _velocity.y << ", " << _velocity.z << "] av: [" << _angularVelocity.x << ", " << _angularVelocity.y << ", " << _angularVelocity.z << "] vav: [" << _vectorAngularVelocity.x << ", " << _vectorAngularVelocity.y << ", " << _vectorAngularVelocity.z << "]" << endl;
}

float Glider::GetHeight()
{
	float height = getPos().z;
	return height;
}

// Debug
void Glider::DrawDebug(const glm::vec3& angularVelocity)
{
	Draw2::AddFunction([this, angularVelocity]() mutable {
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
					float red[] = { 1.f, 0.f, 0.f, 0.25f };
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
					float green[] = { 0.f, 1.f, 0.f, 0.25f };
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
					float blue[] = { 0.f, 0.f, 1.f, 0.25f };
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
					float red[] = { 1.f, 0.f, 0.f, 0.5f };
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
					float green[] = { 0.f, 1.f, 0.f, 0.5f };
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
					float blue[] = { 0.f, 0.f, 1.f, 0.5f };
					Draw2::SetColorClass<ShaderLinePM>(blue);
					glm::vec4 direct(0.f, 0.f, 1.f, 0.f);
					direct = getMatrix() * direct;
					direct *= 100.f;
					const float vertices[] = { 0.f, 0.f, 0.f, direct.x, direct.y, direct.z };
					const unsigned int count = 2;
					Draw2::drawLines(vertices, count);
				}
			}

			// Force
			{
				// X
				{
					float red[] = { 1.f, 0.9f, 0.9f, 1.f };
					Draw2::SetColorClass<ShaderLinePM>(red);
					glm::vec4 direct(angularVelocity.x, angularVelocity.y, angularVelocity.z, 1.f);
					direct = getMatrix() * direct;
					direct *= 100.f;
					const float vertices[] = { 0.f, 0.f, 0.f, direct.x, direct.y, direct.z };
					const unsigned int count = 2;
					Draw2::drawLines(vertices, count);
				}

				// Y
				{
					float green[] = { 0.9f, 1.f, 0.9f, 1.f };
					Draw2::SetColorClass<ShaderLinePM>(green);
					glm::vec4 direct(angularVelocity.x, angularVelocity.y, angularVelocity.z, 1.f);
					direct = getMatrix() * direct;
					direct *= 100.f;
					const float vertices[] = { 0.f, 0.f, 0.f, direct.x, direct.y, direct.z };
					const unsigned int count = 2;
					Draw2::drawLines(vertices, count);
				}

				// Z
				{
					float blue[] = { 0.9f, 0.9f, 1.f, 1.f };
					Draw2::SetColorClass<ShaderLinePM>(blue);
					glm::vec4 direct(angularVelocity.x, angularVelocity.y, angularVelocity.z, 1.f);
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
