#pragma once
#include <cstdint>
#include <memory>
#include <util/typealiases.hpp>
#include <GLFW/glfw3.h>
#include <cmp\camera.hpp>
#include <cmp\light.hpp>
#include <vector>

namespace ECS {

struct GameContext_t;

struct RenderSystem_t {
    explicit RenderSystem_t(uint32_t w, uint32_t h);
    ~RenderSystem_t();

    bool update(const GameContext_t& g);
    void drawAllEntities(const VecEntities_t& entities) const;
	void drawLightSource(const LightSource_t& light)  const;
    // void drawEntity(const Entity_t& e) const;

	float vertices[180] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	std::vector<LightSource_t> lights{};

	/*static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void processInput(GLFWwindow* window);*/

	//CAMERA
	//inline static glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	//inline static glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	//inline static glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	inline static Camera camera{};

	inline static float deltaTime = 0.0f;	// Time between current frame and last frame
	double lastFrame = 0.0f; // Time of last frame
	double lastX = 400, lastY = 300;
	inline static float yaw = 0;
	inline static float pitch = 0;
	inline static float fov = 45.0;

private :
    const uint32_t m_w { 0 }, m_h { 0 };
	void mouse_move(GLFWwindow* window);
	void init();
	bool firstMouse = true;
	GLFWwindow* window;
	//glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	//SHADERS
	Shader myShader{ "Vertex.glsl", "Fragment.glsl" };
	Shader lightShader{ "lightningVertex.glsl", "lightningFragment.glsl" };
	
};
}