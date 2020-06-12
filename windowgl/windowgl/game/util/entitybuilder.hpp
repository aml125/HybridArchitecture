#pragma once

#include <ecs\cmp\entity.hpp>
#include <game\man\gamemanager.hpp>
#include <game\cmp\physics.hpp>
#include <game\cmp\boxcollider.hpp>
#include <game\sys\ia.hpp>

namespace GM {
	struct EntityBuilder {
		explicit EntityBuilder() = default;

		//static ECS::Entity_t& buildSoldier(ECS::EntityManager_t& em);
		//static ECS::Entity_t& buildTower(ECS::EntityManager_t& em);
		//static ECS::Entity_t& buildGround(ECS::EntityManager_t& em);
		
		static ECS::Entity_t& buildEntityWithModelAndCollision(GM::GameManager& em, const std::string& modelPath, const glm::vec3& colliderLength, const glm::vec3& colliderOffset);
		static ECS::Entity_t& buildFullEntity(GM::GameManager& em, const glm::vec3& position, const std::string& modelPath, const glm::vec3& colliderLength, const glm::vec3& colliderOffset);
		static ECS::Entity_t& buildNPC(GameManager& gm, const glm::vec3& position, const std::string& modelPath, unsigned int patternNumber, IASystem_t& iaSystem);
	};
}