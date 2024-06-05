#include "entitybuilder.hpp"
#include <game\cmp\model.hpp>
#include <game\util\log.hpp>
#include <game\cmp\instantiatedModel.hpp>
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

namespace GM {
	ECS::Entity_t& EntityBuilder::buildEntityWithModelAndCollision(GameManager& gm, const std::string& modelPath,
		const glm::vec3& colliderLength, const glm::vec3& colliderOffset)
	{
		auto& em = gm.entityMan;
		ECS::Entity_t& e = em.createEntity();
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

	ECS::Entity_t& EntityBuilder::buildInstantiatedEntityWithModel(GameManager& gm, const std::string& modelPath)
	{
		auto& em = gm.entityMan;
		ECS::Entity_t& e = em.createEntity();
		auto& ph = em.createComponent<PhysicsComponent_t>(e.entityID);
		e.addComponent(ph);
		ph.speed.x = ph.speed.y = ph.speed.z = 0;

		//Add model
		InstantiatedModel_t& mod = em.createComponent<InstantiatedModel_t>(e.entityID);
		mod.loadModel(modelPath);
		e.addComponent(mod);

		return e;
	}

	ECS::Entity_t& EntityBuilder::buildInstantiatedEntityWithModelAndCollision(GameManager& gm, const std::string& modelPath,
		const glm::vec3& colliderLength, const glm::vec3& colliderOffset)
	{
		auto& em = gm.entityMan;
		ECS::Entity_t& e = em.createEntity();
		auto& ph = em.createComponent<PhysicsComponent_t>(e.entityID);
		e.addComponent(ph);
		ph.speed.x = ph.speed.y = ph.speed.z = 0;

		//Add model
		/*InstantiatedModel_t& mod = em.createComponent<InstantiatedModel_t>(e.entityID);
		mod.loadModel(modelPath);
		e.addComponent(mod);*/

		//Add box colider
		BoxCollider_t& bc = em.createComponent<BoxCollider_t>(e.entityID);
		e.addComponent(bc);
		bc.length = colliderLength;
		bc.offset = colliderOffset;

		return e;
	}

	ECS::Entity_t& EntityBuilder::buildFullEntity(GameManager& gm, const glm::vec3& position,
		const std::string& modelPath, const glm::vec3& colliderLength, const glm::vec3& colliderOffset)
	{
		auto& e = buildEntityWithModelAndCollision(gm, modelPath, colliderLength, colliderOffset);
		auto* phy = e.getComponent<PhysicsComponent_t>();
		if (phy == nullptr) {
			GM::Log::log("EntityBuilder__buildFullEntity() ERROR created entity has no physics system. SOMETHING GONE TERRIBLY WRONG");
			exit(-1);
		}
		//Set position
		phy->position = position;
		return e;
	}

	ECS::Entity_t& EntityBuilder::buildFullInstantiatedEntity(GameManager& gm, const glm::vec3& position,
		const std::string& modelPath)
	{
		auto& e = buildInstantiatedEntityWithModel(gm, modelPath);
		auto* phy = e.getComponent<PhysicsComponent_t>();
		if (phy == nullptr) {
			GM::Log::log("EntityBuilder__buildFullEntity() ERROR created entity has no physics system. SOMETHING GONE TERRIBLY WRONG");
			exit(-1);
		}
		//Set position
		phy->position = position;
		return e;
	}

	ECS::Entity_t& EntityBuilder::buildFullInstantiatedEntityWithColl(GameManager& gm, const glm::vec3& position,
		const std::string& modelPath, const glm::vec3& colliderLength, const glm::vec3& colliderOffset)
	{
		auto& e = buildInstantiatedEntityWithModelAndCollision(gm, modelPath, colliderLength, colliderOffset);
		auto* phy = e.getComponent<PhysicsComponent_t>();
		if (phy == nullptr) {
			GM::Log::log("EntityBuilder__buildFullEntity() ERROR created entity has no physics system. SOMETHING GONE TERRIBLY WRONG");
			exit(-1);
		}
		//Set position
		phy->position = position;
		return e;
	}

	//NANOSUIT Scale: 0.1
	ECS::Entity_t& EntityBuilder::buildNPC(GameManager& gm, const glm::vec3& position,
		const std::string& modelPath, unsigned int patternNumber, IASystem_t& iaSystem) {
		constexpr glm::vec3 cLength1{ 1, 1.55f, 0.5f };
		constexpr glm::vec3 cOffset1{ 0, 0.78f, 0 };
		ECS::Entity_t& e1 = GM::EntityBuilder::buildFullInstantiatedEntityWithColl(gm, position, modelPath, cLength1, cOffset1);
		
		//IA
		GM::IA_t& ia = gm.entityMan.createComponent<GM::IA_t>(e1.entityID);
		ia.patternNumber = patternNumber;
		iaSystem.fm.addCharacter(ia, gm.entityMan.getComponents<GM::IA_t>());

		//Physics
		auto* phy1 = e1.getComponent<GM::PhysicsComponent_t>();
		phy1->scale.x = phy1->scale.y = phy1->scale.z = 1.0f;
		phy1->gravity = true;
		return e1;
	}
	ECS::Entity_t& EntityBuilder::buildNPCWithoutIaColl(GameManager& gm, const glm::vec3& position,
		const std::string& modelPath, unsigned int patternNumber, IASystem_t& iaSystem) {
		constexpr glm::vec3 cLength1{ 1, 1.55f, 0.5f };
		constexpr glm::vec3 cOffset1{ 0, 0.78f, 0 };
		ECS::Entity_t& e1 = GM::EntityBuilder::buildFullInstantiatedEntity(gm, position, modelPath);

		//IA
		/*GM::IA_t& ia = gm.entityMan.createComponent<GM::IA_t>(e1.entityID);
		ia.patternNumber = patternNumber;
		iaSystem.fm.addCharacter(ia, gm.entityMan.getComponents<GM::IA_t>());*/

		//Physics
		auto* phy1 = e1.getComponent<GM::PhysicsComponent_t>();
		phy1->scale.x = phy1->scale.y = phy1->scale.z = 1.0f;
		phy1->gravity = false;
		return e1;
	}
	void EntityBuilder::buildPattern(GameManager& gm, IASystem_t& iaSys, unsigned int totalSlots, unsigned int depth, float separation, const std::string& modelPath, const glm::vec3& initialPosition)
	{
		Pattern& pat = iaSys.fm.createPattern(gm.entityMan);
		int rowSlots = totalSlots / depth;
		int auxRow = rowSlots;
		int auxDepth = depth;
		if (rowSlots % 2 == 1) {
			auxRow--;
		}
		if (depth % 2 == 1) {
			auxDepth--;
		}
		float xPos = -(separation * auxRow / 2);
		float zPos = -(separation * auxDepth / 2);
		float initXPos = xPos;
		float initZPos = zPos;
		for (int i = 0; i < depth; i++) {
			for (int j = 0; j < rowSlots; j++) {
				auto& loc = pat.slots.emplace_back();
				loc.position = { xPos, 0, zPos };
				buildNPC(gm, (initialPosition + glm::vec3{ xPos, 0, zPos }), modelPath, pat.patternNumber, iaSys);
				xPos += separation;
			}
			xPos = initXPos;
			zPos += separation;
		}
	}

	void EntityBuilder::buildPatternWithoutIaColl(GameManager& gm, IASystem_t& iaSys, unsigned int totalSlots, unsigned int depth, float separation, const std::string& modelPath, const glm::vec3& initialPosition)
	{
		//Pattern& pat = iaSys.fm.createPattern(gm.entityMan);
		int rowSlots = totalSlots / depth;
		int auxRow = rowSlots;
		int auxDepth = depth;
		if (rowSlots % 2 == 1) {
			auxRow--;
		}
		if (depth % 2 == 1) {
			auxDepth--;
		}
		float xPos = -(separation * auxRow / 2);
		float zPos = -(separation * auxDepth / 2);
		float initXPos = xPos;
		float initZPos = zPos;
		for (int i = 0; i < depth; i++) {
			for (int j = 0; j < rowSlots; j++) {
				//auto& loc = pat.slots.emplace_back();
				//loc.position = { xPos, 0, zPos };
				buildNPCWithoutIaColl(gm, (initialPosition + glm::vec3{ xPos, 0, zPos }), modelPath, 0, iaSys);
				xPos += separation;
			}
			xPos = initXPos;
			zPos += separation;
		}
	}
}