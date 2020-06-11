#pragma once

#include <glm\glm\ext\vector_float3.hpp>

#include <vector>
#include <game\cmp\ia.hpp>
#include <ecs/man/entitymanager.hpp>
#include <game\icmp\pattern.hpp>

namespace GM {
	struct FormationManager {
		bool addCharacter(IA_t& ia, std::vector<IA_t>& assignments);
		void removeCharacter(IA_t& ia, std::vector<IA_t>& assignments);
		void updateSlots(std::vector<IA_t>& assignments);
		void ressetAnchorsToCenterOfMass(const std::vector<IA_t>& assignments, const ECS::EntityManager_t& em);

		Pattern& createPattern(ECS::EntityManager_t& em);
		Pattern& getPattern(unsigned int index) {
			if (index >= patterns.size()) {
				std::cout << "ERROR: Pattern& FormationManager::getPattern(...) Index (" << std::to_string(index) << ") out of range" << std::endl;
			}
			else {
				return patterns[index];
			}
		}

	private:
		std::vector<Pattern> patterns;
		void setTarget(Target& target, const Location& location);
	};
}