#pragma once

#include <game/sys/system.hpp>
#include <game\cmp\ia.hpp>
#include <game\cmp\physics.hpp>
#include <game/man/formation.hpp>
#include <game\util\timemeasure.hpp>

namespace GM {
	struct Steering {
		glm::vec3 aceleration{};
		glm::vec3 rotation{};
	};

	struct IASystem_t : System_t {
		void update(ECS::EntityManager_t& em) override;
		FormationManager fm{};

	private:
		Steering arrive(const IA_t& ia, const PhysicsComponent_t& phy, bool& arrived);
		void setOrientation(glm::vec3& orientation, const glm::vec3& velocity);
		void stateMachineUpdate(ECS::EntityManager_t& em, IA_t& ia, PhysicsComponent_t& phy);
		bool targetHasMoved(IA_t& ia, PhysicsComponent_t& phy);
		bool setFormationTarget(ECS::EntityManager_t& em, IA_t& ia, const PhysicsComponent_t& targetPhy);
		PhysicsComponent_t& findOtherFormation(ECS::EntityManager_t& em, IA_t& myFormation);
		unsigned int findClosestEnemy(const ECS::EntityManager_t& em, const glm::vec3& pos, unsigned int formation);
		void setTargetOnRangeOfAttack(const ECS::EntityManager_t& em, IA_t& ia);
		int totalArrived = 0;
		TimeMeasure tm;
	};
}