#pragma once

#include <ecs\cmp\entity.hpp>
#include <ecs\man\entitymanager.hpp>
#include <game\cmp\physics.hpp>
#include <game\cmp\boxcollider.hpp>

namespace GM {
	struct EntityBuilder {
		explicit EntityBuilder() = default;

		//static ECS::Entity_t& buildSoldier(ECS::EntityManager_t& em);
		//static ECS::Entity_t& buildTower(ECS::EntityManager_t& em);
		//static ECS::Entity_t& buildGround(ECS::EntityManager_t& em);
		
		static ECS::Entity_t& buildEntityWithModelAndCollision(ECS::EntityManager_t& em, const std::string& modelPath, const glm::vec3& colliderLength, const glm::vec3& colliderOffset);
		static ECS::Entity_t& buildFullEntity(ECS::EntityManager_t& em, const glm::vec3& position, const std::string& modelPath, const glm::vec3& colliderLength, const glm::vec3& colliderOffset);
	};
}