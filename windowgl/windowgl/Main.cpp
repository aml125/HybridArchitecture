#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <Shader.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <man\entitymanager.hpp>
#include <sys\physics.hpp>
#include <sys\collision.hpp>
#include <sys/render.hpp>
#include <cmp/model.hpp>
#include <input.hpp>
#include <cmp/pointLight.hpp>



constexpr glm::vec3 cubePositions[] = {
  glm::vec3(0.0f,  0.0f,  0.0f),
  glm::vec3(2.0f,  5.0f, -15.0f),
  glm::vec3(-1.5f, -2.2f, -2.5f),
  glm::vec3(-3.8f, -2.0f, -12.3f),
  glm::vec3(2.4f, -0.4f, -3.5f),
  glm::vec3(-1.7f,  3.0f, -7.5f),
  glm::vec3(1.3f, -2.0f, -2.5f),
  glm::vec3(1.5f,  2.0f, -2.5f),
  glm::vec3(1.5f,  0.2f, -1.5f),
  glm::vec3(-1.3f,  1.0f, -1.5f)
};

constexpr unsigned int kSCRWIDTH{ 1024 };
constexpr unsigned int kSCRHEIGHT{ 768 };

//const std::string MESA_PATH = "C:\\Users\\tonet\\Desktop\\OpenGl\\windowgl\\x64\\Debug\\models\\mesa\\mesa.fbx";
//const std::string NANOSUIT_PATH = "C:\\Users\\tonet\\Desktop\\OpenGl\\windowgl\\x64\\Debug\\models\\nanosuit\\nanosuit.obj";
//const std::string DICE_PATH = "C:\\Users\\tonet\\Desktop\\OpenGl\\windowgl\\x64\\Debug\\models\\dice\\dice.fbx";

const std::string MESA_PATH = "models\\mesa\\mesa.fbx";
const std::string NANOSUIT_PATH = "models\\nanosuit\\nanosuit.obj";
const std::string DICE_PATH = "models\\dice\\dice.fbx";
const std::string SUELO_PATH = "models\\suelo\\suelo.obj";
const std::string TORRE_PATH = "models\\torre\\torre.obj";

// positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};

ECS::Entity_t* player;

void up() {
	player->phy->speed.z = 0.5;
}

void left() {
	player->phy->speed.x = 0.5;
}

void right() {
	player->phy->speed.x = -0.5;
}

void down() {
	player->phy->speed.z = -0.5;
}

void space() {
	player->phy->speed.y = 0.6;
}

float lastPushTime = 0.0f;
void c() {
	if (ECS::RenderSystem_t::time > lastPushTime + 2) {
		ECS::RenderSystem_t::drawCollisions = !ECS::RenderSystem_t::drawCollisions;
		lastPushTime = ECS::RenderSystem_t::time;
	}
}

void l() {
	if (ECS::RenderSystem_t::time > lastPushTime + 2) {
		ECS::RenderSystem_t::drawPointLights = !ECS::RenderSystem_t::drawPointLights;
		lastPushTime = ECS::RenderSystem_t::time;
	}
}

int main()
{
	ECS::Window_t window{ kSCRWIDTH, kSCRHEIGHT };
	ECS::RenderSystem_t Render(window);
	ECS::InputSystem_t Input(window);
	ECS::EntityManager_t EntityMan;

	//Set callbacks
	Input.upKeyDown = up;
	Input.downKeyDown = down;
	Input.leftKeyDown = left;
	Input.rightKeyDown = right;
	Input.spaceKeyDown = space;
	Input.cKeyDown = c;
	Input.lKeyDown = l;
	
	EntityMan.createEntity(glm::vec3(0, 0, 0), SUELO_PATH);
	EntityMan.createEntity(glm::vec3(10, 20, 0), NANOSUIT_PATH);
	EntityMan.createEntity(glm::vec3(0, 30, 0), TORRE_PATH);

	ECS::VecEntities_t& vec = EntityMan.getEntities();
	vec[0].collider.length.x = 20.75f;
	vec[0].collider.length.z = 23.66f;
	vec[0].collider.length.y = 0.75f;
	vec[0].collider.offset.x = 0.375f;
	vec[0].collider.offset.y = 0.375f;
	vec[0].collider.offset.z = -0.495;
	
	vec[1].collider.offset.y = 0.78f;
	vec[1].collider.length.x = 1;
	vec[1].collider.length.y = 1.55f;
	vec[1].collider.length.z = 0.5f;
	vec[1].phy->scale.x = vec[1].phy->scale.y = vec[1].phy->scale.z = 0.1f;
	vec[1].phy->gravity = true;
	player = &vec[1];

	vec[2].phy->gravity = true;
	vec[2].collider.length.x = 1.25;
	vec[2].collider.length.y = 3;
	vec[2].collider.length.z = 2;
	vec[2].collider.offset = { 0.625f, 1.5f, -1.0f };
	vec[2].phy->scale = { 0.0015f, 0.0015f, 0.0015f };

	ECS::PointLight_t pt({3, 1.7, -1.5}, { 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f });
	Render.lights.push_back(pt);

	ECS::PhysicsSystem_t Physics;
	ECS::CollisionSystem_t Collision;
	while (Render.update(EntityMan)) {
		Input.update();
		Physics.update(EntityMan);
		Collision.update(EntityMan);
	}
	Render.~RenderSystem_t();
	char c = getchar();
	return 0;
}