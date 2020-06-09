#pragma once

#include <game/sys/system.hpp>
#include <game\cmp\ia.hpp>
#include <game\cmp\physics.hpp>
#include <game/man/formation.hpp>

namespace GM {
	struct Steering {
		glm::vec3 aceleration{};
		glm::vec3 rotation{};
	};

	struct IASystem_t : System_t {
		void update(ECS::EntityManager_t& em) override;
		Steering arrive(const IA_t& ia, const PhysicsComponent_t& phy);
		FormationManager fm{};

		//WARNING Makes a full copy of the slots
		std::vector<Location>& setPatternSlots(std::vector<Location>& sp) {
			fm.pattern.slots = sp;
			return fm.pattern.slots;
		}
	};
}