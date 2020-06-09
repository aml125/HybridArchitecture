#pragma once
#include <game\icmp\datatypes.hpp>
#include <vector>
#include <game/cmp/ia.hpp>

namespace GM {

	struct AnchorPoint {
		PhysicsComponent_t phy{ 999 };
		IA_t ia{ 999 }; //TODO Refactor, and make it an entity
	};

	struct Pattern {
		std::vector<Location> slots{};
		AnchorPoint anchorPoint{};
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
		void resetVec3(glm::vec3& v) const {
			v.x = 0;
			v.y = 0;
			v.z = 0;
		}
	};
}