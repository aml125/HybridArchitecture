#include <game/sys/ia.hpp>
#include <ecs/man/entitymanager.hpp>
#include <glm\glm\ext\vector_float3.hpp>
#include <glm\glm\geometric.hpp>
#include <game\sys\physics.hpp>

namespace GM {
	PhysicsSystem_t auxP{}; //TODO Borrar esta locura
	void IASystem_t::update(ECS::EntityManager_t& em) {
		//Reset the anchor point to the center of mass of the formation
		fm.calculateRealMassCenter(em.getComponents <IA_t>(), em);
		fm.pattern.anchorPoint.phy.position = fm.pattern.realMassCenter.position;
		fm.pattern.anchorPoint.phy.rotation = fm.pattern.realMassCenter.orientation;

		//Execute the steering behaviours to get to the targets
		for (IA_t& ia : em.getComponents<IA_t>()) {
			auto* phy = em.getEntity(ia.entityID).getComponent<PhysicsComponent_t>();
			if (phy == nullptr) {
				//std::cout << "IASystem_t::update() ERROR cant have ia without Physics System\n";
				return;
			}
			auto steering = arrive(ia, *phy); //TODO, It may be copying the values, but compiler nowadays optimizes return values
			phy->aceleration = steering.aceleration;
			phy->rotationAceleration = steering.rotation;
		}
		//std::cout << std::endl;
		//TODO Refactor. Moving pattern anchor point
		auto steering = arrive(fm.pattern.anchorPoint.ia, fm.pattern.anchorPoint.phy);
		fm.pattern.anchorPoint.phy.aceleration = steering.aceleration;
		fm.pattern.anchorPoint.phy.rotationAceleration = steering.rotation;
		auxP.moveObject(fm.pattern.anchorPoint.phy);

		//std::cout << "AnchorPos: x:" << fm.pattern.anchorPoint.phy.position.x << " y:" << fm.pattern.anchorPoint.phy.position.y << " z:" << fm.pattern.anchorPoint.phy.position.z << std::endl << std::endl;

		//Execute the pattern manager to set the new targets of the formation
		fm.updateSlots(em.getComponents<IA_t>());
		//std::cout << "\n------------------------------------------\n";
	}

	Steering IASystem_t::arrive(const IA_t& ia, const PhysicsComponent_t& phy)
	{
		Steering steering; //Return data
		
		glm::vec3 direction = ia.target.position - phy.position;
		float distance = glm::length(direction);
		//std::cout << "Distance: " << distance << std::endl;

		//If we are in the target radius, we arived
		if (distance < ia.targetRadius) {
			steering.aceleration = -phy.speed;
			//std::cout << "arrived" << std::endl;
			return steering;
		}

		//If outside of the slow radius, go full speed
		float targetSpeed = 0;
		if (distance > ia.slowRadius) {
			targetSpeed = ia.maxSpeed;
			//std::cout << "outside slow radius\n";
		}
		else {
			//Else calculate speed
			targetSpeed = ia.maxSpeed * distance / ia.slowRadius;
			//std::cout << "Inside slow radius\n";
		}

		//Target velocity combines speed and direction
		glm::vec3 targetVelocity = glm::normalize(direction);
		targetVelocity *= targetSpeed;

		//Calculate aceleration to get to the target velocity. Takes into account acceleration time
		steering.aceleration = (targetVelocity - phy.speed) / ia.timeToTarget;
		
		//Check if acceleration is too fast
		if (glm::length(steering.aceleration) > ia.maxAcceleration) {
			steering.aceleration = glm::normalize(steering.aceleration);
			steering.aceleration *= ia.maxAcceleration;
			//std::cout << "Too fast\n";
		}
		/*std::cout << "Position: x:" << phy.position.x << " y:" << phy.position.y << " z:" << phy.position.z << std::endl;
		std::cout << "Target: x:" << ia.target.position.x << " y:" << ia.target.position.y << " z:" << ia.target.position.z << std::endl;
		std::cout << "Acell: x:" << steering.aceleration.x << " y:" << steering.aceleration.y << " z:" << steering.aceleration.z << std::endl << std::endl;*/
		return steering;
	}

	
}