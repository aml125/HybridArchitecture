#include "gamemanager.hpp"
#include <game\sys\render.hpp>

bool GM::GameManager::update()
{
	for (auto* sys : systems) {
		sys->update(entityMan);
	}

	return !RenderSystem_t::windowShouldClose;
}
