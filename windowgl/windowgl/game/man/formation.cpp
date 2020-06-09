#include <game/man/formation.hpp>
#include <game\cmp\physics.hpp>

namespace GM {
	//void FormationManager::update(ECS::EntityManager_t& em)
	//{
	//	auto& a = em.getComponents<IA_t>();
	//	updateSlots(a);

	//	//Set anchor position at the center of mass
	//	pattern.anchorPoint.position = pattern.realMassCenter.position;
	//	pattern.anchorPoint.orientation = pattern.realMassCenter.orientation;
	//}

	bool FormationManager::addCharacter(IA_t& ia, std::vector<IA_t>& assignments)
	{
		if (pattern.supportsSlots()) {
			ia.slotNumber = pattern.addSlot();
			pattern.calculateDriftOffset(assignments);
			return true;
		}
		return false;
	}

	//The character must have been removed from the IA_T component vector
	void FormationManager::removeCharacter(IA_t& ia, std::vector<IA_t>& assignments)
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
	void FormationManager::updateSlots(std::vector<IA_t>& assignments)
	{
		for (auto& a : assignments) {
			Location location{};
			Location& slotRelPos = pattern.getSlotLocation(a.slotNumber); //Get slot relative position
			
			//Transform slot relative position with the position of the anchor
			location.position = slotRelPos.position// * pattern.anchorPoint.phy.rotation //TODO workout how to put rotation
				+ pattern.anchorPoint.phy.position;
			location.orientation = pattern.anchorPoint.phy.rotation + slotRelPos.orientation;
			
			//Aply the offset drift
			location.position -= pattern.driftOffset.position;
			location.orientation -= pattern.driftOffset.orientation;

			setTarget(a.target, location);
		}
	}

	void FormationManager::calculateRealMassCenter(const std::vector<IA_t>& assignments, const ECS::EntityManager_t& em)
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

	void FormationManager::setTarget(Target& target, const Location& location)
	{
		//std::cout << "setT x:" << location.position.x << " y:" << location.position.y << " z:" << location.position.z << std::endl;
		target.position = location.position;
		target.orientation = location.orientation;
	}

	
}