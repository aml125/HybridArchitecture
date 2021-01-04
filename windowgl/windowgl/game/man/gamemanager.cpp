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

		sincronize();

		return !RenderSystem_t::windowShouldClose;
	}

	void GameManager::sincronize()
	{
		//Set new positions and rotations for the entities
		for (auto& phy : entityMan.getComponents<PhysicsComponent_t>()) {
			/*phy.position.x = phy.nextPosition.x;
			phy.position.y = phy.nextPosition.y;
			phy.position.z = phy.nextPosition.z;

			phy.rotation.x = phy.nextRotation.x;
			phy.rotation.y = phy.nextRotation.y;
			phy.rotation.z = phy.nextRotation.z;*/
			phy.position = phy.nextPosition;
			phy.rotation = phy.nextRotation;
		}
	}
}