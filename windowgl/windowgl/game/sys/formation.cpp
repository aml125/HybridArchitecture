#include "formation.hpp"
#include <game\cmp\physics.hpp>

namespace GM {
	void FormationSystem_t::update(ECS::EntityManager_t& em)
	{
		auto& a = em.getComponents<IA_t>();
		updateSlots(a);

		//Set anchor position at the center of mass
		pattern.anchorPoint.position = pattern.realMassCenter.position;
		pattern.anchorPoint.orientation = pattern.realMassCenter.orientation;
	}

	bool FormationSystem_t::addCharacter(IA_t& ia, std::vector<IA_t>& assignments)
	{
		if (pattern.supportsSlots()) {
			ia.slotNumber = pattern.addSlot();
			pattern.calculateDriftOffset(assignments);
			return true;
		}
		return false;
	}

	//The character must have been removed from the IA_T component vector
	void FormationSystem_t::removeCharacter(IA_t& ia, std::vector<IA_t>& assignments)
	{
		for (auto& a : assignments) {
			if (a.slotNumber > ia.slotNumber) { //ia no se va a modificar, porque es <= que su propio numero
				a.slotNumber--; //Reduce the slot index
			}
		}
		pattern.calculateDriftOffset(assignments);
		//Make removed character slot number useless
		ia.slotNumber = pattern.slots.size();
	}
	void FormationSystem_t::updateSlots(std::vector<IA_t>& assignments)
	{
		for (auto& a : assignments) {
			Location location{};
			Location& slotRelPos = pattern.getSlotLocation(a.slotNumber); //Get slot relative position
			
			//Transform slot relative position with the position of the anchor
			location.position = slotRelPos.position * pattern.anchorPoint.orientation
				+ pattern.anchorPoint.position;
			location.orientation = pattern.anchorPoint.orientation + slotRelPos.orientation;
			
			//Aply the offset drift
			location.position -= pattern.driftOffset.position;
			location.orientation -= pattern.driftOffset.orientation;

			setTarget(a.target, location);
		}
	}

	void FormationSystem_t::calculateRealMassCenter(const std::vector<IA_t>& assignments, const ECS::EntityManager_t& em)
	{
		pattern.resetVec3(pattern.realMassCenter.position);
		pattern.resetVec3(pattern.realMassCenter.orientation);

		//Calculate the average position and orientation
		for (const auto& a : assignments) {
			auto* phy = em.getEntity(a.entityID).getComponent<PhysicsComponent_t>();
			if (phy == nullptr) {
				std::cout << "ERROR: An entity with pattern must have PhysicsSystem\n";
				exit(-1);
			}
			pattern.realMassCenter.position += phy->position;
			pattern.realMassCenter.orientation += phy->rotation;
		}

		std::size_t sz = assignments.size();
		pattern.realMassCenter.position /= sz;
		pattern.realMassCenter.orientation /= sz;
	}

	void FormationSystem_t::setTarget(Target& target, const Location& location)
	{
		target.position = location.position;
		target.orientation = location.orientation;
	}

	void Pattern::calculateDriftOffset(const std::vector<IA_t>& assignments)
	{
		resetVec3(driftOffset.position);
		resetVec3(driftOffset.orientation);

		//Calculate (Anchor point) each slot contribution to the center
		for (auto& a : assignments) {
			auto& location = getSlotLocation(a.slotNumber);
			driftOffset.position += location.position;
			driftOffset.orientation += location.orientation;
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
		return slots[slotNumber];
	}

	//Gives slot relative position and orientation (const version)
	const Location& Pattern::getSlotLocation(unsigned int slotNumber) const
	{
		return slots[slotNumber];
	}
}