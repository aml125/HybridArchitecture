#include <game/sys/ia.hpp>
#include <ecs/man/entitymanager.hpp>
#include <glm\glm\ext\vector_float3.hpp>
#include <glm\glm\geometric.hpp>
#include <game\sys\physics.hpp>

//TODO Refactor: All entities should't have to have a pattern assigned
//TODO make formations not count on y axis, so they walk on the terrain
namespace GM {
	void IASystem_t::update(ECS::EntityManager_t& em) {
		//Execute the steering behaviours to get to the targets
		for (IA_t& ia : em.getComponents<IA_t>()) {
			auto* phy = em.getEntity(ia.entityID).getComponent<PhysicsComponent_t>();
			if (phy == nullptr) {
				std::cout << "IASystem_t::update() ERROR cant have ia without Physics System\n";
				return;
			}
			auto steering = arrive(ia, *phy); //TODO, It may be copying the values, but compiler nowadays optimizes return values
			phy->aceleration = steering.aceleration;
			if (!ia.isAnchorPoint)
				phy->rotation = steering.rotation; //TODO Don set the rotation directly, but with a target and acceleration
		}

		//std::cout << std::endl;

		//fm.pattern.anchorPoint.phy.rotation = steering.rotation; //TODO ¿Anchor point should rotate?

		/*std::cout << "AnchorPos: x:" << fm.getPattern(0).anchorPoint->getComponent<PhysicsComponent_t>()->position.x << " y:" << fm.getPattern(0).anchorPoint->getComponent<PhysicsComponent_t>()->position.y << " z:" << fm.getPattern(0).anchorPoint->getComponent<PhysicsComponent_t>()->position.z << std::endl;
		std::cout << "AnchorPos: x:" << fm.getPattern(0).anchorPoint->getComponent<IA_t>()->target.position.x << " y:" << fm.getPattern(0).anchorPoint->getComponent<IA_t>()->target.position.y << " z:" << fm.getPattern(0).anchorPoint->getComponent<IA_t>()->target.position.z << std::endl << std::endl;*/

		//Execute the pattern manager to set the new targets of the formation
		fm.updateSlots(em.getComponents<IA_t>());

		//std::cout << "\n------------------------------------------\n";

		//Reset the anchor point to the center of mass of the formation
		fm.ressetAnchorsToCenterOfMass(em.getComponents <IA_t>(), em);
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
		setOrientation(steering.rotation, phy.speed); //TODO Don set the rotation directly, but with a target and acceleration
		/*std::cout << "Position: x:" << phy.position.x << " y:" << phy.position.y << " z:" << phy.position.z << std::endl;
		std::cout << "Target: x:" << ia.target.position.x << " y:" << ia.target.position.y << " z:" << ia.target.position.z << std::endl;
		std::cout << "Acell: x:" << steering.aceleration.x << " y:" << steering.aceleration.y << " z:" << steering.aceleration.z << std::endl << std::endl;*/
		return steering;
	}

	void IASystem_t::setOrientation(glm::vec3& orientation, const glm::vec3& velocity)
	{
		if (glm::length(velocity) > 0) {
			orientation.y = std::atan2(velocity.x, velocity.z);
		}
	}

	
}