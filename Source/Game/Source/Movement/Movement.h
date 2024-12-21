// ◦ Xyz ◦
#pragma once

#include <memory>
#include <Game.h>

class Camera;
class Map;
class Glider;

namespace Engine {
	class Callback;
}

class Movement final : public Engine::Game
{
public:
	Movement() = default;
	~Movement() = default;
	std::filesystem::path getSourcesDir() override { return "..\\..\\Source\\Resources\\Files\\Movement"; }

	void init() override;
	void close() override;
	void update() override;
	void draw() override;
	void resize() override;

	void InitСameras();
	void InitPhysic();
	void InitCallback();
	bool Load();
	void Save();
	void GenerateMap();

private:
	Glider* GetPlayerGlider();

public:
	std::shared_ptr<Map> _mapGame;
	std::shared_ptr<Camera> _camearCurrent;
	std::shared_ptr<Engine::Callback> _callbackPtr;

	bool _cameraType = false;
};
