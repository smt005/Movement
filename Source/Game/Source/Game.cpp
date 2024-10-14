// ◦ Xyz ◦

#include "Movement/Movement.h"
#define NAME_GAME Movement

Engine::Game::Uptr Engine::Game::GetGame(const std::string& params) {
	Engine::Game::Uptr gameUptr(new NAME_GAME());
	return gameUptr;
}
