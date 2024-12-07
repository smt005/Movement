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
#include <Physics/Physics.h>
#include <Common/PrintTemplate.h>
//#include "glm/ext/matrix_transform.hpp"

#include <Common/Help.h>
#include <Draw/Camera/CameraControlOutside.h>
#include <Draw2/Draw2.h>
#include <Draw2/Shader/ShaderDefault.h>
#include <Draw2/Shader/ShaderLine.h>
#include "Object/Map.h"
#include <Object/Model.h>
#include "ImGuiManager/UI.h"

using namespace glider;

void Glider::EnableControl(bool enable)
{
	if (!_callbackPtr && enable) {
		_callbackPtr = std::make_shared<Engine::Callback>(Engine::CallbackType::PINCH_KEY, [this](const Engine::CallbackEventPtr& callbackEventPtr) {
			if (Engine::Callback::pressKey(Engine::VirtualKey::F1)) {
				ResetPosition();
			}

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
				//Move(MoveDirect::LEFT, kForce);
				rotateZ += -0.01;
			}
			if (Engine::Callback::pressKey(Engine::VirtualKey::D)) {
				//Move(MoveDirect::RIGHT, kForce);
				rotateZ += 0.01;
			}
			if (Engine::Callback::pressKey(Engine::VirtualKey::R)) {
				//Move(MoveDirect::FORVARD_HORIZONT, kForce);
			}
			if (Engine::Callback::pressKey(Engine::VirtualKey::F)) {
				//Move(MoveDirect::BACK_HORIZONT, kForce);
			}

			//...
			static glm::vec3 vectorTorqueForce(0.f, 0.f, 0.f);

			if (Engine::Callback::pressKey(Engine::VirtualKey::I)) {
				vectorTorqueForce.y += 0.001f;
				_torqueForce = vectorTorqueForce;
			}
			if (Engine::Callback::pressKey(Engine::VirtualKey::K)) {
				vectorTorqueForce.y -= 0.001f;
				_torqueForce = vectorTorqueForce;
			}
			if (Engine::Callback::pressKey(Engine::VirtualKey::L)) {
				vectorTorqueForce.x += 0.001f;
				_torqueForce = vectorTorqueForce;
			}
			if (Engine::Callback::pressKey(Engine::VirtualKey::J)) {
				vectorTorqueForce.x -= 0.001f;
				_torqueForce = vectorTorqueForce;
			}

			// Height

			if (Engine::Callback::pressKey(Engine::VirtualKey::H)) {
				paramsPtr->height -= 10;
			}

			// Type
			if (Engine::Callback::pressKey(Engine::VirtualKey::CONTROL)) {
				if (Engine::Callback::pressKey(Engine::VirtualKey::VK_0)) {
					_torqueForceType = Engine::Physics::Force::ACCELERATION;
				}
				else if (Engine::Callback::pressKey(Engine::VirtualKey::VK_1)) {
					_torqueForceType = Engine::Physics::Force::FORCE;
				}
				else if (Engine::Callback::pressKey(Engine::VirtualKey::VK_2)) {
					_torqueForceType = Engine::Physics::Force::IMPULSE;
				}
				else if (Engine::Callback::pressKey(Engine::VirtualKey::VK_3)) {
					_torqueForceType = Engine::Physics::Force::VELOCITY_CHANGE;
				}
			}
		});

		_callbackPtr->add(Engine::CallbackType::PRESS_KEY, [this](const Engine::CallbackEventPtr& callbackEventPtr) {
			if (Engine::Callback::pressKey(Engine::VirtualKey::Y)) {
				_b_ = !_b_;
			}
		});
	}

	if (!enable && _callbackPtr) {
		_callbackPtr.reset();
	}
}

glm::vec3 ExtractDirectionVector(const glm::mat4& mat) {
	// Третий столбец матрицы (индекс 2): вектор направления
	glm::vec3 direction(mat[2][0], mat[2][1], mat[2][2]);

	// Нормализация вектора (если требуется единичный вектор)
	return glm::normalize(direction);
}

glm::vec3 RotateVectorByMatrix(const glm::vec3& vec, const glm::mat4& mat) {
	// Преобразование в vec4 с w = 0.0 (для исключения трансляции)
	glm::vec4 rotatedVec = mat * glm::vec4(vec, 0.0f);

	// Возврат результата как vec3
	return glm::vec3(rotatedVec);
}

void Glider::Move(const MoveDirect direct, const float kForce)
{
	//glm::vec3 cameraDirect = ExtractDirectionVector(getMatrix());
	//glm::vec3 cameraDirect = RotateVectorByMatrix({0.f, 1.f, 0.f}, getMatrix());
	glm::vec4 _cameraDirect_ = getMatrix()* glm::vec4(0.f, 1.f, 0.f, 0.f);
	glm::vec3 cameraDirect = { _cameraDirect_.x, _cameraDirect_.y, 0.f };

	cout << "cameraDirect: ["; help::PrintXYZ(cameraDirect, ", ", 0); cout << "]" << endl;
	cameraDirect.z = 0.f;
	cameraDirect = glm::normalize(cameraDirect);

	float force = GetParams().force;
	glm::vec3 vectorForce = { 0.f, 0.f, 0.f };

	switch (direct)
	{
	case MoveDirect::FORVARD:
		vectorForce += cameraDirect * force * kForce;
		_force += vectorForce;
		//cout << "direct: " << (int)direct << "["; help::PrintXYZ(cameraDirect, ", ", 0); cout << "] ["; help::PrintXYZ(vectorForce, ", ", 1); cout << "] => ";
		//vectorForce = {  1.f, 0.f, 0.f };
		break;

	case MoveDirect::BACK:
		//vectorForce = { -1.f,0.f, 0.f };
		vectorForce -= cameraDirect * force * kForce;
		_force += vectorForce;
		//cout << "direct: " << (int)direct << "["; help::PrintXYZ(cameraDirect, ", ", 0); cout << "] ["; help::PrintXYZ(vectorForce, ", ", 1); cout << "] = >";
		break;

	/*case MoveDirect::LEFT:
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
		*/
	case MoveDirect::FORVARD_HORIZONT:
		//vectorForce.x += (cameraDirect.x * force * kForce);
		//vectorForce.y += (cameraDirect.y * force * kForce);
		break;

	case MoveDirect::BACK_HORIZONT:
		//vectorForce.x -= (cameraDirect.x * force * kForce);
		//vectorForce.y -= (cameraDirect.y * force * kForce);
		break;
	}

	//addForce(vectorForce);


	//_force += glm::vec3(vectorForce.x, vectorForce.y, 0.f);
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
	Rotate();

	DrawDebug();

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
		float forceZ = delta * params.liftingForce - velocityZ  * params.liftingDamping;
		glm::vec3 vector = { 0.f, 0.f, forceZ };
		_force += vector;
	}

	// Стабилизация
	/*{
		glm::mat4x4  mat = getMatrix();
		glm::quat q = glm::quat_cast(mat);
		glm::quat target;
		glm::quat delta = glm::normalize(glm::inverse(q) * target);

		float torqueStrength = 15.0f; // Сила возвращения к целевой ориентации
		float damping = 0.15f;        // Демпфирование угловой скорости

		glm::vec3 axis = glm::eulerAngles(delta) * 3.14159f / 180.f;
		float angle = glm::angle(delta);

		// Рассчет корректирующего торка
		glm::vec3 correctiveTorque = axis * (-angle * torqueStrength);

		// Учет угловой скорости (демпфирование)
		glm::vec3 angularVelocity = GetAngularVelocity();
		glm::vec3 dampingTorque = angularVelocity * (-damping);

		// Применение суммарного торка
		_torqueForce = correctiveTorque + dampingTorque;
	}*/

	// Рыскание
	{
		Stabilization();
	}

	//...
	addTorque(_torqueForce);

	//cout << " FINAL: ["; help::PrintXYZ(_force, ", ", 1); cout << "]" << endl;
	addForce(_force);
	ResetForce();
}

void Glider::Rotate()
{
	auto c = Camera::GetLink().Direct();
	_camera_ = glm::normalize(glm::vec3(c.x, c.y, 0.f));
}

void Glider::Stabilization()
{
	glm::mat4x4  mat = getMatrix();
	
	//static float rotateZ = 0.f;

	{
		auto AngleXY = [](const auto& vec0, const auto& vec1) {
			// Расчёт длины векторов
			auto Magnitude = [](const auto& vec) {
				return std::sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
			};

			// Основной вектор: вычисление угла
			float magnitudeProduct = Magnitude(vec0) * Magnitude(vec1);
			if (magnitudeProduct == 0.0f) return 0.0f; // Защита от деления на ноль

			float angleMain = ((vec0[0] * vec1[1]) - (vec0[1] * vec1[0])) / magnitudeProduct;

			// Вспомогательный вектор: вычисление угла
			auto vecOrthogonal = glm::vec2(vec0[1], -vec0[0]); // Перпендикулярный вектор
			float magnitudeOrthogonal = Magnitude(vecOrthogonal) * Magnitude(vec1);
			if (magnitudeOrthogonal == 0.0f) return 0.0f;

			float angleAuxiliary = ((vecOrthogonal[0] * vec1[1]) - (vecOrthogonal[1] * vec1[0])) / magnitudeOrthogonal;

			// Коррекция знака основного угла
			if (angleMain < 0.0f) {
				if (angleAuxiliary < 0.0f) angleMain = -1.0f - (1.0f + angleMain);
			}
			else if (angleMain > 0.0f) {
				if (angleAuxiliary < 0.0f) angleMain = 1.0f + (1.0f - angleMain);
			}

			return angleMain / 2.0f;
		};

		glm::vec4 _direct4_ = /*mat **/ glm::vec4(0.f, -1.f, 0.f, 0.f);
		glm::vec3 _direct3_ = glm::normalize(glm::vec3(_direct4_.x, _direct4_.y, 0.f));

		auto camDirect = Camera::GetLink().Direct();
		glm::vec3 _camdirect3_ = glm::normalize(glm::vec3(camDirect.x, camDirect.y, 0.f));

		volatile static bool _bbb_ = true;
		if (_bbb_) {
			rotateZ = AngleXY(_camdirect3_, _direct3_) * 3.14159265358979323846264338327950288f;
			//rotateZ = -AngleXY(_camdirect3_, _direct3_) * 3.14159265358979323846264338327950288f;
		}
	}
		
	mat = glm::rotate(mat, rotateZ, { 0.f, 0.f, 1.f });

	help::log(rotateZ);

	glm::quat q = glm::quat_cast(mat);

	float torqueStrength = 15.0f; // Сила возвращения к целевой ориентации
	float damping = 0.15f;        // Демпфирование угловой скорости

	glm::vec3 axis = glm::eulerAngles(q) * 3.14159265358979323846264338327950288f / 180.f;
	float angle = glm::angle(q);

	// Рассчет корректирующего торка
	glm::vec3 correctiveTorque = axis * (-angle * torqueStrength);

	// Учет угловой скорости (демпфирование)
	glm::vec3 angularVelocity = GetAngularVelocity();
	glm::vec3 dampingTorque = angularVelocity * (-damping);

	// Применение суммарного торка
	_torqueForce += correctiveTorque + dampingTorque;
}

/*glm::vec3 Glider::Rotate()
{
	auto AngleXY = [](const auto& vec0, const auto& vec1) {
		// Расчёт длины векторов
		auto Magnitude = [](const auto& vec) {
			return std::sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
		};

		// Основной вектор: вычисление угла
		float magnitudeProduct = Magnitude(vec0) * Magnitude(vec1);
		if (magnitudeProduct == 0.0f) return 0.0f; // Защита от деления на ноль

		float angleMain = ((vec0[0] * vec1[1]) - (vec0[1] * vec1[0])) / magnitudeProduct;

		// Вспомогательный вектор: вычисление угла
		auto vecOrthogonal = glm::vec2(vec0[1], -vec0[0]); // Перпендикулярный вектор
		float magnitudeOrthogonal = Magnitude(vecOrthogonal) * Magnitude(vec1);
		if (magnitudeOrthogonal == 0.0f) return 0.0f;

		float angleAuxiliary = ((vecOrthogonal[0] * vec1[1]) - (vecOrthogonal[1] * vec1[0])) / magnitudeOrthogonal;

		// Коррекция знака основного угла
		if (angleMain < 0.0f) {
			if (angleAuxiliary < 0.0f) angleMain = -1.0f - (1.0f + angleMain);
		}
		else if (angleMain > 0.0f) {
			if (angleAuxiliary < 0.0f) angleMain = 1.0f + (1.0f - angleMain);
		}

		return angleMain / 2.0f;
	};


	glm::vec3 directCameraV3 = Camera::GetLink().Direct();
	glm::vec3 directCamera = glm::normalize(glm::vec3(directCameraV3.x, directCameraV3.y, 0.f));

	glm::vec4 directV4 = getMatrix() * glm::vec4(0.f, 1.f, 0.f, 0.f);
	glm::vec3 direct = glm::normalize(glm::vec3(directV4.x, directV4.y, 0.f));
	
	volatile static float factor = 0.01f;
	volatile static float damping = 0.01f;

	glm::vec3 angularVelocity = GetAngularVelocity();
	float dampingTorque = angularVelocity.z * (-damping);

	float angle = AngleXY(directCamera, direct) * factor + dampingTorque;

	//cout << "[" << directCamera.x << ", " << directCamera.y << "]" "[" << direct.x << ", " << direct.y << "]" << "angle: " << angle << endl;

	volatile static bool _b_ = true;
	if (_b_) {
		return glm::vec3(0.f, 0.f, angle);
	}
	else {
		return glm::vec3(0.f, 0.f, 0.f);
	}
}*/

void Glider::Update()
{
}

float Glider::GetHeight()
{
	static glm::vec3 directDown(0.f, 0.f, -1.f);
	glm::vec3 pos = getPos();
	pos.z += 1.f;
	const glm::vec3 hitPos = Engine::Physics::Raycast(pos, directDown);
	float height = pos.z - hitPos.z;
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

// Debug
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

			// Force
			{
				// X
				{
					float white[] = { 1.f, 1.f, 1.f, 1.f };
					Draw2::SetColorClass<ShaderLinePM>(white);
					
					static glm::vec3 cameraZZZ;
					if (_b_) {
						glm::vec3 camera = _camera_;
						camera *= 100.f;
						cameraZZZ = camera;
						const float vertices[] = { 0.f, 0.f, 0.f, camera.x, camera.y, camera.z };
						const unsigned int count = 2;
						Draw2::drawLines(vertices, count);
					}
					else {
						const float vertices[] = { 0.f, 0.f, 0.f, cameraZZZ.x, cameraZZZ.y, cameraZZZ.z };
						const unsigned int count = 2;
						Draw2::drawLines(vertices, count);
					}
				}

				// Y
				/*{
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
				}*/
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
