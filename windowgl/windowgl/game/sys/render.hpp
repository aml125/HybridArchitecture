#pragma once
#include <cstdint>
#include <memory>
#include <game/util/typealiases.hpp>
#include <game/rcmp/camera.hpp>
#include <vector>
#include <game/cmp/pointLight.hpp>
#include <game/cmp/directionalLight.hpp>
#include <game/rcmp/window.hpp>
#include <ecs/man/entitymanager.hpp>
#include <game\cmp\boxcollider.hpp>
#include <game/cmp/model.hpp>
#include <GLFW/glfw3.h>

namespace GM {

struct GameContext_t;

struct RenderSystem_t {
    explicit RenderSystem_t(Window_t window);
    ~RenderSystem_t();

    bool update(const ECS::EntityManager_t& g);

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
	void drawLightSource(const PointLight_t& light)  const;
	void drawLights() const;
	void drawCollisionBoxes(const ECS::EntityManager_t& em, const std::vector<BoxCollider_t>& entities) const;
	void drawCollisionBox(const BoxCollider_t& box, const glm::vec3 position) const;
	
	Window_t& window;
	BoxRenderer_t collisionRenderer;

	
	glm::mat4 view;
	glm::mat4 projection;

	//SHADERS
	Shader myShader{ "Vertex.glsl", "Fragment.glsl" };
	Shader lightShader{ "lightningVertex.glsl", "lightningFragment.glsl" };
	
};
}