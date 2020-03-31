#pragma once
#include <cstdint>
#include <memory>
#include <util/typealiases.hpp>
#include <GLFW/glfw3.h>
#include <cmp\camera.hpp>
#include <vector>
#include <cmp\pointLight.hpp>
#include <cmp/directionalLight.hpp>
#include <cmp\window.hpp>

namespace ECS {

struct GameContext_t;

struct RenderSystem_t {
    explicit RenderSystem_t(Window_t window);
    ~RenderSystem_t();

    bool update(const GameContext_t& g);

	//Lights
	std::vector<PointLight_t> lights{};
	DirectionalLight_t sunLight{ {-0.2f, -1.0f, -0.3f}, { 0.05f, 0.05f, 0.05f}, {0.4f, 0.4f, 0.4f}, {0.5f, 0.5f, 0.5f} };

	//CAMERA
	inline static Camera camera{};

	inline static float deltaTime = 0.0f;	// Time between current frame and last frame
	double lastFrame = 0.0f; // Time of last frame
	inline static float yaw = 0;
	inline static float pitch = 0;
	inline static float fov = 45.0;

private :
    
	void setLightInformation() const;
	void drawAllEntities(const VecEntities_t& entities) const;
	void drawLightSource(const PointLight_t& light)  const;
	void drawLights() const;
	void drawCollisionBoxes(const VecEntities_t& entities) const;
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