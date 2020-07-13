#include "gamemanager.hpp"
#include <game\sys\render.hpp>
#include <game\util\log.hpp>

GM::GameManager::GameManager()
{
	std::atexit(GM::Log::flush);
}

bool GM::GameManager::update()
{
	for (auto* sys : systems) {
		sys->update(entityMan);
	}

	return !RenderSystem_t::windowShouldClose;
}
