#include "Weapon.h"
#include <Core.h>
#include <Log.h>
#include <Object/Object.h>
#include <Object/Map.h>

Weapon::Bullet::Bullet(Object* argPtr)
	: ptr(argPtr)
	, time(Engine::Core::currentTime())
{}

void Weapon::Update()
{
	if (_bullets.empty()) {
		return;
	}

	Bullet& bullet = _bullets.front();
	if (Engine::Core::currentTime() - bullet.time < bullet.lifeTimeParam) {
		return;
	}

	Map::Ptr currentMap = Map::GetFirstCurrentMapPtr();
	if (!currentMap) {
		return;
	}

	Log("Weapon: Dead: pos: {}", bullet.ptr->getPos());

	currentMap->AddToRemoveObject(bullet.ptr);
	_bullets.pop_front();
}

void Weapon::Shot(const glm::vec3& pos, const glm::vec3& vec)
{
	double currentTime = Engine::Core::currentTime();
	double dTime = Engine::Core::currentTime() - _time;
	if (dTime < _rateParam) {
		return;
	}
	_time = currentTime;

	Log("Weapon: Shot: pos: {}, vec: {}", pos, vec);

	Map::Ptr currentMap = Map::GetFirstCurrentMapPtr();
	if (!currentMap) {
		return;
	}
	
	const glm::vec3 weaponPos(pos.x, pos.y, pos.z + 15.f);
	Object& bullet = currentMap->addObjectToPos("BulletCone", weaponPos);

	bullet.setTypeActorPhysics(Engine::Physics::Type::CONVEX);
	bullet.createActorPhysics();

	const glm::vec3 forceVec = vec * _force;
	bullet.addForce(forceVec, Engine::Physics::Force::IMPULSE);

	_bullets.push_back(&bullet);
}
