#pragma once
#include <game\icmp\datatypes.hpp>
#include <vector>
#include <game/cmp/ia.hpp>
#include <ecs\cmp\entity.hpp>

namespace GM {

	struct Pattern {
		std::vector<Location> slots{};
		Location formationMassCenter{};
		Location realMassCenter{};
		DriftOffset driftOffset{};
		ECS::Entity_t* anchorPoint;
		unsigned int ocupiedSlots = 0;
		unsigned int patternNumber = 0;
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
		static void resetVec3(glm::vec3& v) {
			v.x = 0;
			v.y = 0;
			v.z = 0;
		}
	};
}