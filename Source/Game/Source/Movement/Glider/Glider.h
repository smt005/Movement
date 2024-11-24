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
	{}

	void EnableControl(bool enable);
	void Move(const MoveDirect direct, const float kForce = 1.f);
	const glider::Params& GetParams();

public:
	void action() override;

private:
	float GetHeight();
	void DrawDebug(const glm::vec3& angularVelocity);

private:
	Engine::Callback::Ptr _callbackPtr;
	glider::Params::Ptr paramsPtr;
	glm::vec3 _torqueForce = { 0.f, 0.f, 0.f };
	Engine::Physics::Force _torqueForceType = Engine::Physics::Force::VELOCITY_CHANGE;
};
