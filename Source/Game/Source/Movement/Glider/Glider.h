// ◦ Xyz ◦
#pragma once

#include "glm/vec3.hpp"
#include "../../Engine/Source/Callback/Callback.h"
#include "Object/Object.h"
#include "GliderParams.h"

class Glider : public Object {
public:
	typedef std::shared_ptr<Glider> Ptr;

	enum class MoveDirect {
		NONE,
		FORVARD, BACK, LEFT, RIGHT, TOP, DOWN,
		FORVARD_HORIZONT, BACK_HORIZONT
	};

public:
	Glider(const string& name, const string& modelName, const vec3& pos)
		:Object(name, modelName, pos, Engine::Physics::Type::CONVEX)
	{
		setMass(10.f);
	}

	void EnableControl(bool enable);
	void Move(const MoveDirect direct, const float kForce = 1.f);
	glm::vec3 Rotate();
	const glider::Params& GetParams();

	void action() override;
	void Stabilization();
	void Update();

private:
	float GetHeight();
	void ResetForce();

private:
	void DrawDebug();

private:
	Engine::Callback::Ptr _callbackPtr;
	glider::Params::Ptr paramsPtr;
	glm::vec3 _force = { 0.f, 0.f, 0.f };
	glm::vec3 _torqueForce = { 0.f, 0.f, 0.f };
	float rotateZ = 0;
	Engine::Physics::Force _torqueForceType = Engine::Physics::Force::VELOCITY_CHANGE;
};
