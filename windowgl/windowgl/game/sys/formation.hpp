#pragma once

#include <glm\glm\ext\vector_float3.hpp>
#include <game/sys/system.hpp>
#include <vector>
#include <game\cmp\ia.hpp>

namespace GM {
	struct Location {
		glm::vec3 position;
		glm::vec3 orientation;
	};

	struct DriftOffset {
		glm::vec3 position;
		glm::vec3 orientation;
	};

	struct Pattern {
		std::vector<Location> slots{};
		Location anchorPoint{};
		Location formationMassCenter{};
		Location realMassCenter{};
		DriftOffset driftOffset{};
		unsigned int ocupiedSlots = 0;
		bool supportsSlots() const
		{
			return slots.size() > ocupiedSlots;
		}
		int addSlot() {
			return ocupiedSlots++;
		}

		void calculateDriftOffset(const std::vector<IA_t>& assignments);
		const Location& getSlotLocation(unsigned int slotNumber) const;
		Location& getSlotLocation(unsigned int slotNumber);
		void resetVec3(glm::vec3& v) const{
			v.x = 0;
			v.y = 0;
			v.z = 0;
		}
	};

	struct FormationSystem_t : System_t {
		// Heredado vía System_t
		virtual void update(ECS::EntityManager_t& em) override;
		bool addCharacter(IA_t& ia, std::vector<IA_t>& assignments);
		void removeCharacter(IA_t& ia, std::vector<IA_t>& assignments);
		void updateSlots(std::vector<IA_t>& assignments);
		void calculateRealMassCenter(const std::vector<IA_t>& assignments, const ECS::EntityManager_t& em);

		Pattern pattern{};

	private:
		void setTarget(Target& target, const Location& location);
	};
}