#include "entitybuilder.hpp"
#include <game\cmp\model.hpp>
//
//ECS::Entity_t& GM::EntityBuilder::buildSoldier(ECS::EntityManager_t& em)
//{
//	
//}
//
//ECS::Entity_t& GM::EntityBuilder::buildTower(ECS::EntityManager_t& em)
//{
//	return ECS::Entity_t();
//}
//
//ECS::Entity_t& GM::EntityBuilder::buildGround(ECS::EntityManager_t& em)
//{
//	return ECS::Entity_t();
//}

ECS::Entity_t& GM::EntityBuilder::buildEntityWithModelAndCollision(ECS::EntityManager_t& em, const std::string& modelPath, 
	const glm::vec3& colliderLength, const glm::vec3& colliderOffset)
{
	ECS::Entity_t &e = em.createEntity();
	auto& ph = em.createComponent<PhysicsComponent_t>(e.entityID);
	e.addComponent(ph);
	ph.speed.x = ph.speed.y = ph.speed.z = 0;

	//Add model
	Model_t& mod = em.createComponent<Model_t>(e.entityID);
	mod.loadModel(modelPath);
	e.addComponent(mod);

	//Add box colider
	BoxCollider_t& bc = em.createComponent<BoxCollider_t>(e.entityID);
	e.addComponent(bc);
	bc.length = colliderLength;
	bc.offset = colliderOffset;

	return e;
}

ECS::Entity_t& GM::EntityBuilder::buildFullEntity(ECS::EntityManager_t& em, const glm::vec3& position, 
	const std::string& modelPath, const glm::vec3& colliderLength, const glm::vec3& colliderOffset)
{
	auto& e = buildEntityWithModelAndCollision(em, modelPath, colliderLength, colliderOffset);
	auto* phy = e.getComponent<PhysicsComponent_t>();
	if (phy == nullptr) {
		std::cout << "EntityBuilder__buildFullEntity() ERROR created entity has no physics system. SOMETHING GONE TERRIBLY WRONG\n";
		exit(-1);
	}
	//Set position
	phy->position = position;
	return e;
}
