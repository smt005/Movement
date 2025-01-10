#pragma once

#include <glm/vec3.hpp>
#include <Object/Object.h>
#include <deque>
#include <memory>

class Weapon final {
private:
	struct Bullet
	{
		Bullet(Object* argPtr);
		Object* ptr; 
		double time;
		double lifeTimeParam = 3000;
	};

public:
	void Update();
	void Shot(const glm::vec3& pos, const glm::vec3& vec);

private:
	float _force = 500.f;
	double _time;

	double _rateParam = 250;

	std::deque<Bullet> _bullets;
};
