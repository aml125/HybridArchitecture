#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <Shader.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <game\rcmp\window.hpp>
#include <ecs\util\typealiases.hpp>
#include <game\cmp\boxcollider.hpp>
#include <game\cmp\physics.hpp>
#include <game\sys\physics.hpp>
#include <game\sys\collision.hpp>
#include <game\sys\render.hpp>
#include <game\sys\input.hpp>
#include <game\util\entitybuilder.hpp>



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

const std::string MESA_PATH = "assets\\models\\mesa\\mesa.fbx";
const std::string NANOSUIT_PATH = "assets\\models\\nanosuit\\nanosuit.obj";
const std::string DICE_PATH = "assets\\models\\dice\\dice.fbx";
const std::string SUELO_PATH = "assets\\models\\suelo\\suelo.obj";
const std::string TORRE_PATH = "assets\\models\\torre\\torre.obj";

// positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};

GM::PhysicsComponent_t* player;

void up() {
	player->speed.z = 0.5;
}

void left() {
	player->speed.x = 0.5;
}

void right() {
	player->speed.x = -0.5;
}

void down() {
	player->speed.z = -0.5;
}

void space() {
	player->speed.y = 0.6;
}

float lastPushTime = 0.0f;
void c() {
	if (GM::RenderSystem_t::time > lastPushTime + 2) {
		GM::RenderSystem_t::drawCollisions = !GM::RenderSystem_t::drawCollisions;
		lastPushTime = GM::RenderSystem_t::time;
	}
}

void l() {
	if (GM::RenderSystem_t::time > lastPushTime + 2) {
		GM::RenderSystem_t::drawPointLights = !GM::RenderSystem_t::drawPointLights;
		lastPushTime = GM::RenderSystem_t::time;
	}
}

int main()
{
	GM::Window_t window{ kSCRWIDTH, kSCRHEIGHT };
	GM::RenderSystem_t Render(window);
	GM::InputSystem_t Input(window);
	ECS::EntityManager_t EntityMan;

	//Set callbacks
	Input.upKeyDown = up;
	Input.downKeyDown = down;
	Input.leftKeyDown = left;
	Input.rightKeyDown = right;
	Input.spaceKeyDown = space;
	Input.cKeyDown = c;
	Input.lKeyDown = l;
	

	glm::vec3 cLength0{ 20.75f, 0.75f,  23.66f };
	glm::vec3 cOffset0{ 0.375f, 0.375f, -0.495 };
	ECS::Entity_t& e0 = GM::EntityBuilder::buildFullEntity(EntityMan, glm::vec3(0, 0, 0), SUELO_PATH, cLength0, cOffset0);

	
	glm::vec3 cLength1{ 1, 1.55f, 0.5f };
	glm::vec3 cOffset1{ 0, 0.78f, 0 };
	ECS::Entity_t& e1 = GM::EntityBuilder::buildFullEntity(EntityMan, glm::vec3(10, 1, 0), NANOSUIT_PATH, cLength1, cOffset1);
	auto* phy1 = e1.getComponent<GM::PhysicsComponent_t>();
	phy1->scale.x = phy1->scale.y = phy1->scale.z = 0.1f;
	phy1->gravity = true;
	player = phy1;

	glm::vec3 cLength2{ 1.25f, 3, 2 };
	glm::vec3 cOffset2{ 0.625f, 1.5f, -1.0f };
	ECS::Entity_t& e2 = GM::EntityBuilder::buildFullEntity(EntityMan, glm::vec3(0, 1, 0), TORRE_PATH, cLength2, cOffset2);
	auto* phy2 = e2.getComponent<GM::PhysicsComponent_t>();
	phy2->gravity = true;
	phy2->scale = { 0.0015f, 0.0015f, 0.0015f };

	GM::PointLight_t pt({3, 1.7, -1.5}, { 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f });
	Render.lights.push_back(pt);

	GM::PhysicsSystem_t Physics;
	GM::CollisionSystem_t Collision;
	while (Render.update(EntityMan)) {
		Input.update();
		Physics.update(EntityMan);
		Collision.update(EntityMan);
	}
	Render.~RenderSystem_t();
	char c = getchar();
	return 0;
}