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
			stateMachineUpdate(em, ia, *phy);
			
		}

		if (totalArrived < 2) {
			//Execute the pattern manager to set the new targets of the formation
			fm.updateSlots(em.getComponents<IA_t>());

			//Reset the anchor point to the center of mass of the formation
			fm.ressetAnchorsToCenterOfMass(em.getComponents <IA_t>(), em);
		}
	}

	Steering IASystem_t::arrive(const IA_t& ia, const PhysicsComponent_t& phy, bool& arrived)
	{
		Steering steering; //Return data
		arrived = false;
		
		glm::vec3 direction = ia.target.position - phy.position;
		float distance = glm::length(direction);
		//std::cout << "Distance: " << distance << std::endl;

		//If we are in the target radius, we arived
		if (distance <= ia.targetRadius) {
			steering.aceleration = -phy.speed;
			//std::cout << "arrived" << std::endl;
			arrived = true;
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

	void IASystem_t::stateMachineUpdate(ECS::EntityManager_t& em, IA_t& ia, PhysicsComponent_t& phy)
	{
		bool arrived = false;
		Steering* steering;
		//Hard coded state machine
		switch (ia.state) {
		case ARRIVE:
			steering = &arrive(ia, phy, arrived);
			phy.aceleration = steering->aceleration;
			phy.rotation = steering->rotation; //TODO Don set the rotation directly, but with a target and acceleration

			if (arrived) {
				ia.state = WAIT;
			}
			break;
		case ANCHOR_WAIT:

			break;
		case ANCHOR_ARRIVE:
		{
			bool setFormation = setFormationTarget(em, ia, findOtherFormation(em, ia));
			bool arrived = false;
			steering = &arrive(ia, phy, arrived);
			phy.aceleration = steering->aceleration;
			if (setFormation && arrived) {
				std::cout << "Anchor arrived\n";
				ia.state = ANCHOR_WAIT;
				totalArrived++;
				//Cancel aceleration and speed of the formation anchor
				phy.aceleration = { 0, 0, 0 };
				phy.speed = { 0, 0, 0 };
			}
			break;
		}
		case WAIT:
			if (ia.isAnchorPoint) {
				ia.state = ANCHOR_ARRIVE;
			}
			else if (totalArrived == 2) {
				//find closest enemy
				ia.enemyID = findClosestEnemy(em, phy.position, ia.patternNumber);
				ia.state = GO;
			}
			else if (targetHasMoved(ia, phy)) {
				ia.state = ARRIVE;
			}
			break;
		case GO:
			//Chase the enemy
			setTargetOnRangeOfAttack(em, ia);
			steering = &arrive(ia, phy, arrived);
			phy.aceleration = steering->aceleration;
			phy.rotation = steering->rotation;
			if (arrived) {
				ia.state = ATTACK;
			}
			break;
		case ATTACK:
			ia.state = BACK;
			break;
		case BACK:
			fm.updateSlot(ia);
			steering = &arrive(ia, phy, arrived);
			phy.aceleration = steering->aceleration;
			phy.rotation = steering->rotation;
			if (arrived) {
				ia.state = GO;
			}
			break;
		}
	}
	bool IASystem_t::targetHasMoved(IA_t& ia, PhysicsComponent_t& phy)
	{
		glm::vec3 direction = ia.target.position - phy.position;
		float distance = glm::length(direction);
		return distance > ia.targetRadius;
	}

	//Sets the target for the formation and returns true if already arrived
	bool IASystem_t::setFormationTarget(ECS::EntityManager_t& em, IA_t& ia, const PhysicsComponent_t& targetPhy)
	{
		auto* myPhy = em.getEntity(ia.entityID).getComponent<PhysicsComponent_t>();
		glm::vec3 direction = targetPhy.position - myPhy->position;
		ia.target.position.x = targetPhy.position.x;
		if (direction.z > 0) {
			ia.target.position.z = targetPhy.position.z - 3; //- or + x to let x space between formations
		}
		else {
			ia.target.position.z = targetPhy.position.z + 3;
		}

		if (myPhy->position.x == targetPhy.position.x) {
			return true;
		}
		return false;
	}

	//TODO: Optimize to dont search through all ia list every time
	PhysicsComponent_t& IASystem_t::findOtherFormation(ECS::EntityManager_t& em, IA_t& myFormation)
	{
		for (IA_t& ia : em.getComponents<IA_t>()) {
			if (ia.isAnchorPoint && ia.entityID != myFormation.entityID) {
				return *em.getEntity(ia.entityID).getComponent<PhysicsComponent_t>();
			}
		}
		std::cout << "EROR: PhysicsComponent_t& IASystem_t::findOtherFormation(...) No other formation found\n";
		exit(-1);
		return *em.getEntity(myFormation.entityID).getComponent<PhysicsComponent_t>(); //JUST TO AVOID NON RETURN ERROR
	}

	unsigned int IASystem_t::findClosestEnemy(const ECS::EntityManager_t& em, const glm::vec3& pos, unsigned int formation) {
		bool first = true;
		float min = 0;
		unsigned int targetID = 999;
		for (auto& ia : em.getComponents<IA_t>()) {
			if (ia.patternNumber != formation && ia.patternNumber < 2) {
				auto* phy = em.getEntity(ia.entityID).getComponent<PhysicsComponent_t>();
				float distance = std::abs(glm::length(phy->position - pos));
				if (first || distance < min) {
					min = distance;
					targetID = phy->entityID;
					first = false;
				}
			}
		}
		std::cout << "TARGET: " << targetID << std::endl;
		return targetID;
	}

	void IASystem_t::setTargetOnRangeOfAttack(const ECS::EntityManager_t& em, IA_t& ia) {
		auto* enemyPhy = em.getEntity(ia.enemyID).getComponent<PhysicsComponent_t>();
		auto* myPhy = em.getEntity(ia.entityID).getComponent<PhysicsComponent_t>();
		if (enemyPhy == nullptr || myPhy == nullptr) {
			std::cout << "ERROR: void setTargetOnRangeOfAttack(const ECS::EntityManager_t& em, IA_t& ia) IA of enemy has no physic component\n";
			exit(-1);
		}
		glm::vec3 distance = enemyPhy->position - myPhy->position;
		if (distance.z > 0) {
			ia.target.position.z = enemyPhy->position.z - 0.5;
		}
		else {
			ia.target.position.z = enemyPhy->position.z + 0.5;
		}
		ia.target.position.x = enemyPhy->position.x;
	}
}