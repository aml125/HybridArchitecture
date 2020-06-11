#include <game/man/formation.hpp>
#include <game\cmp\physics.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace GM {
	//void FormationManager::update(ECS::EntityManager_t& em)
	//{
	//	auto& a = em.getComponents<IA_t>();
	//	updateSlots(a);

	//	//Set anchor position at the center of mass
	//	pattern->anchorPoint.position = pattern->realMassCenter.position;
	//	pattern->anchorPoint.orientation = pattern->realMassCenter.orientation;
	//}

	bool FormationManager::addCharacter(IA_t& ia, std::vector<IA_t>& assignments)
	{
		auto& pattern = getPattern(ia.patternNumber);
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
			if ((!a.isAnchorPoint) && a.slotNumber > ia.slotNumber) { //ia no se va a modificar, porque es <= que su propio numero
				a.slotNumber--; //Reduce the slot index
			}
		}
		auto& pattern = getPattern(ia.patternNumber);
		pattern.calculateDriftOffset(assignments);
		//Make removed character slot number useless
		ia.slotNumber = pattern.slots.size();
	}

	//Updates the slots to the new position of the anchor using the relative position of each slot
	void FormationManager::updateSlots(std::vector<IA_t>& assignments)
	{
		for (auto& a : assignments) {
			if (!a.isAnchorPoint) {
				auto& pattern = getPattern(a.patternNumber);
				auto* anchorPhy = pattern.anchorPoint->getComponent<PhysicsComponent_t>();
				if (anchorPhy == nullptr) {
					std::cout << "ERROR: void FormationManager::updateSlots(...) pattern has no phisycs component";
					exit(-1);
				}
				Location location{};
				Location& slotRelPos = pattern.getSlotLocation(a.slotNumber); //Get slot relative position

				//Transform slot relative position with the position of the anchor
				location.position = slotRelPos.position //TODO workout how to put rotation
					+ anchorPhy->position;
				location.orientation = anchorPhy->rotation + slotRelPos.orientation;

				//Aply the offset drift
				location.position -= pattern.driftOffset.position;
				location.orientation -= pattern.driftOffset.orientation;

				setTarget(a.target, location);
			}
		}
	}

	//Calculates the real Mass center of all patterns
	void FormationManager::ressetAnchorsToCenterOfMass(const std::vector<IA_t>& assignments, const ECS::EntityManager_t& em)
	{
		//Resset centers of all patterns
		for (auto& pattern : patterns) {
			pattern.resetVec3(pattern.realMassCenter.position);
			pattern.resetVec3(pattern.realMassCenter.orientation);
		}
		

		//Calculate the average position and orientation of all patterns
		std::vector<float> positions(patterns.size());
		for (const auto& a : assignments) {
			if (!a.isAnchorPoint) {
				auto& pattern = getPattern(a.patternNumber);
				auto* phy = em.getEntity(a.entityID).getComponent<PhysicsComponent_t>();
				if (phy == nullptr) {
					std::cout << "ERROR: An entity with pattern must have PhysicsSystem\n";
					exit(-1);
				}
				pattern.realMassCenter.position += phy->position;
				pattern.realMassCenter.orientation += phy->rotation;
			}
		}

		//Final division and position resset
		for (auto& pattern : patterns) {
			std::size_t sz = pattern.slots.size();
			pattern.realMassCenter.position /= sz;
			pattern.realMassCenter.orientation /=sz;
			auto* phy = pattern.anchorPoint->getComponent<PhysicsComponent_t>();
			phy->position = pattern.realMassCenter.position;
			phy->rotation = pattern.realMassCenter.orientation;
		}
	}

	Pattern& FormationManager::createPattern(ECS::EntityManager_t& em)
	{
		auto* ap = &em.createEntity();
		auto& phy = em.createComponent<PhysicsComponent_t>(ap->entityID);
		IA_t& aia = em.createComponent<IA_t>(ap->entityID);
		aia.isAnchorPoint = true;
		ap->addComponent(phy);
		ap->addComponent(aia);
		Pattern& pat = patterns.emplace_back();
		pat.anchorPoint = ap;
		return pat;
	}

	void FormationManager::setTarget(Target& target, const Location& location)
	{
		//std::cout << "setT x:" << location.position.x << " y:" << location.position.y << " z:" << location.position.z << std::endl;
		target.position = location.position;
		target.orientation = location.orientation;
	}

	
}