// ◦ Xyz ◦
#pragma once

#include <memory>
#include <Game.h>

class Camera;
class Map;

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
	bool Load();
	void Save();
	void GenerateMap();

public:
	std::shared_ptr<Map> _mapGame;
	std::shared_ptr<Camera> _camearCurrent;
};
