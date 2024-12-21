// ◦ Xyz ◦
#pragma once

#include <memory>
#include <glm/vec3.hpp>
#include "Object/Object.h"

namespace Engine {
	class Callback;
}
using CallbackPtr = std::shared_ptr<Engine::Callback>;

namespace glider {
	struct Params;
}
using ParamsPtr = std::shared_ptr<glider::Params>;

class Glider : public Object {
public:
	typedef std::shared_ptr<Glider> Ptr;

	enum class MoveDirect {
		NONE,
		FORVARD, BACK, LEFT, RIGHT, TOP, DOWN
	};

public:
	Glider(const string& name, const string& modelName, const vec3& pos)
		: Object(name, modelName, pos, Engine::Physics::Type::CONVEX)
	{
		setMass(10.f);
	}

	void action() override;

	void EnableControl(bool enable);
	void Move(const MoveDirect direct, const float kForce = 1.f);
	const glider::Params& GetParams();

private:
	void Stabilization();
	float GetHeight();
	void ResetForce();

private:
#if _DEBUG
	void DrawDebug();
	void ResetPosition();
#endif

private:
	glm::vec3 _force = { 0.f, 0.f, 0.f };
	glm::vec3 _torqueForce = { 0.f, 0.f, 0.f };
	CallbackPtr _callbackPtr;
	ParamsPtr paramsPtr;
};
