#include <game/sys/ia.hpp>
#include <ecs/man/entitymanager.hpp>
#include <glm\glm\ext\vector_float3.hpp>
#include <glm\glm\geometric.hpp>

namespace GM {
	void IASystem_t::update(ECS::EntityManager_t& em) {
		for (IA_t& ia : em.getComponents<IA_t>()) {
			auto* phy = em.getEntity(ia.entityID).getComponent<PhysicsComponent_t>();
			if (phy == nullptr) {
				std::cout << "IASystem_t::update() ERROR cant have ia without Physics System\n";
				return;
			}
			auto steering = arrive(ia, *phy); //TODO, It may be copying the values, but compiler nowadays optimizes return values
			phy->aceleration = steering.aceleration;
		}
	}

	Steering IASystem_t::arrive(const IA_t& ia, const PhysicsComponent_t& phy)
	{
		Steering steering; //Return data
		
		glm::vec3 direction = ia.target.position - phy.position;
		float distance = glm::length(direction);

		//If we are in the target radius, we arived
		if (distance < ia.targetRadius) {
			return steering;
		}

		//If outside of the slow radius, go full speed
		float targetSpeed = 0;
		if (distance > ia.slowRadius) {
			targetSpeed = ia.maxSpeed;
		}
		else {
			//Else calculate speed
			targetSpeed = ia.maxSpeed * distance / ia.slowRadius;
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
		}

		return steering;
	}

	
}