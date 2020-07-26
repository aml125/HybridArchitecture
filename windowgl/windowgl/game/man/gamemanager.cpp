#include "gamemanager.hpp"
#include <game\sys\render.hpp>
#include <game\util\log.hpp>
#include <thread>


namespace GM {
	GameManager::GameManager(RenderSystem_t* ren)
	{
		render = ren;
		std::atexit(GM::Log::flush);
	}

	void executeRender(RenderSystem_t* render, ECS::EntityManager_t* entityMan) {
		render->update(*entityMan);
	}

	void executeSystems(std::vector<System_t*>* systems, ECS::EntityManager_t* entityMan) {
		for (auto* sys : *systems) {
			sys->update(*entityMan);
		}
	}

	bool GameManager::update()
	{
		
		//std::thread thread(executeSystems, &systems, &entityMan);
		
		executeRender(render, &entityMan);
		executeSystems(&systems, &entityMan);
		//thread.join();

		return !RenderSystem_t::windowShouldClose;
	}
}