#include <game/icmp/pattern.hpp>
#include <iostream>
#include <string>

namespace GM {
	void Pattern::calculateDriftOffset(const std::vector<IA_t>& assignments)
	{
		resetVec3(driftOffset.position);
		resetVec3(driftOffset.orientation);

		//Calculate (Anchor point) each slot contribution to the center
		for (auto& a : assignments) {
			if (!a.isAnchorPoint) {
				auto& location = getSlotLocation(a.slotNumber);
				driftOffset.position += location.position;
				driftOffset.orientation += location.orientation;
			}
		}
		formationMassCenter.position = driftOffset.position;
		formationMassCenter.orientation = driftOffset.orientation;

		auto numAss = assignments.size();
		driftOffset.position /= numAss;
		driftOffset.orientation /= numAss;
	}

	//Gives slot relative position and orientation
	Location& Pattern::getSlotLocation(unsigned int slotNumber)
	{
		if (slots.size() <= slotNumber) {
			std::cout << "ERROR: getSlotLocation(slotNumber) The number of slot (" + std::to_string(slotNumber) + ") does not exist." << std::endl;
			exit(-1);
		}
		return slots[slotNumber];
	}

	//Gives slot relative position and orientation (const version)
	const Location& Pattern::getSlotLocation(unsigned int slotNumber) const
	{
		if (slots.size() <= slotNumber) {
			std::cout << "ERROR: getSlotLocation(slotNumber) The number of slot (" + std::to_string(slotNumber) + ") has not been created." << std::endl;
			exit(-1);
		}
		return slots[slotNumber];
	}
}