#pragma once
#include <cstdint>
#include <memory>
#include <game/util/typealiases.hpp>
#include <game/rcmp/camera.hpp>
#include <vector>
#include <game/rcmp/pointLight.hpp>
#include <game/rcmp/directionalLight.hpp>
#include <game/rcmp/window.hpp>
#include <ecs/man/entitymanager.hpp>
#include <game\cmp\boxcollider.hpp>
#include <game/cmp/model.hpp>
#include <game/sys/system.hpp>
#include <GLFW/glfw3.h>
#include <game\cmp\ia.hpp>
#include <game/util/timemeasure.hpp>
#include <game/cmp/instantiatedModel.hpp>

namespace GM {

struct GameContext_t;

struct RenderSystem_t : System_t {
    explicit RenderSystem_t(Window_t window);
    ~RenderSystem_t();

    void update(ECS::EntityManager_t& g) override;
	void terminateWindow();

	inline static bool windowShouldClose = false;

	//Lights
	std::vector<PointLight_t> lights{};
	DirectionalLight_t sunLight{ {-0.2f, -1.0f, -0.3f}, { 0.05f, 0.05f, 0.05f}, {0.4f, 0.4f, 0.4f}, {0.5f, 0.5f, 0.5f} };

	//CAMERA
	inline static Camera camera{};

	inline static float deltaTime = 0.0f;	// Time between current frame and last frame
	inline static float time = 0.0f;
	double lastFrame = 0.0f; // Time of last frame
	inline static float yaw = 0;
	inline static float pitch = 0;
	inline static float fov = 45.0;
	inline static bool drawCollisions = false;
	inline static bool drawPointLights = true;
	 
private :
    
	void setLightInformation() const;
	void drawAllModels(const ECS::EntityManager_t&, const std::vector<Model_t>& entities) const;
	void drawAllInstantiatedModels(const ECS::EntityManager_t& em, const std::vector<InstantiatedModel_t>& models);
	void drawLightSource(const PointLight_t& light)  const;
	void drawLights() const;
	void drawFormationTargets(const std::vector<IA_t> ias);
	void drawCollisionBoxes(const ECS::EntityManager_t& em, const std::vector<BoxCollider_t>& entities) const;
	void drawCollisionBox(const BoxCollider_t& box, const glm::vec3 position) const;
	
	Window_t& window;
	BoxRenderer_t collisionRenderer; 
	Model_t flag{ 999 };

	
	glm::mat4 view;
	glm::mat4 projection;

	//SHADERS
	Shader myShader{ "shaders/Vertex.glsl", "shaders/Fragment.glsl" };
	Shader lightShader{ "shaders/lightningVertex.glsl", "shaders/lightningFragment.glsl" };
	Shader instancingShader{ "shaders/instancingVertex.glsl", "shaders/Fragment.glsl" };

	TimeMeasure tm{}; 

	unsigned int instanceVBO;
	inline static unsigned int totalInstances = 0;
	glm::mat4* modelMatrixArray = NULL;
};
}