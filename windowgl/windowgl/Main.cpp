#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <game/rcmp/shader.hpp>
#include <game/util/stb_image.h>
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
#include <game\sys\system.hpp>
#include <game\util\entitybuilder.hpp>
#include <game\man\gamemanager.hpp>
#include <game\cmp\ia.hpp>
#include <game/sys/ia.hpp>
#include <game\util\Log.hpp>
#include <game\util\opencl-utils.hpp>
#include <filesystem>

#define TESTING


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

//constexpr unsigned int kSCRWIDTH{ 1024 };
//constexpr unsigned int kSCRHEIGHT{ 768 };
constexpr unsigned int kSCRWIDTH{ 4000 };
constexpr unsigned int kSCRHEIGHT{ 2000 };

const std::string MESA_PATH = "assets\\models\\mesa\\mesa.fbx";
const std::string NANOSUIT_PATH = "assets\\models\\nanosuit\\nanosuit.obj";
const std::string DICE_PATH = "assets\\models\\dice\\dice.fbx";
const std::string SUELO_PATH = "assets\\models\\suelo\\suelo.obj";
const std::string TORRE_PATH = "assets\\models\\torre\\torre.obj";
const std::string CAJA_PATH = "assets\\models\\caja\\caja.obj";
const std::string PINK_KNIGHT_PATH = "assets\\models\\pink_knight\\pink_knight.obj";
const std::string ALABARDERO_PATH = "assets\\models\\alabardero\\alabardero.obj";
const std::string ALABARDERO_ROJO_PATH = "assets\\models\\alabardero_rojo\\alabardero.obj";
const std::string ALABARDERO_AZUL_PATH = "assets\\models\\alabardero_azul\\alabardero.obj";

// positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};

glm::vec3* player;

void up() {
	player->z += 2 * GM::RenderSystem_t::deltaTime;
}

void left() {
	player->x += 2 * GM::RenderSystem_t::deltaTime;
}

void right() {
	player->x += -2 * GM::RenderSystem_t::deltaTime;
}

void down() {
	player->z += -2 * GM::RenderSystem_t::deltaTime;
}

void space() {
	player->y = -0.6 * GM::RenderSystem_t::deltaTime;
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

std::string ExePath() {
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	return std::string(buffer);
}

GM::ocl_args_d_t* GM::System_t::ocl = NULL; // Declare the symbol here

int main(int argc, char *argv[])
{
	std::cout << "Current path: " << ExePath() << std::endl;
	
	// Default params
	std::string gpuName = std::string("Intel");
	int iterations = 100;
	int vars = 4500; // Number of PJ with AI and Collision
	int total_pj = 6000;
	int totalFrames = 20000;

	if (argc >= 6) {
		gpuName = std::string(argv[1]);
		iterations = atoi(argv[2]);
		vars = atoi(argv[3]);
		total_pj = atoi(argv[4]);
		totalFrames = atoi(argv[5]);
	}
	else if (argc > 1) {
		GM::Log::log("ERROR: Not enought arguments. \nUsage: " + std::string(argv[0]) + " gpuName iterations vars total_pj total_frames");
		std::cout << "ERROR: Not enought arguments. \nUsage: " + std::string(argv[0]) + " gpuName iterations vars total_pj total_frames";
		exit(-1);
	}

	int old_total_pj_val = total_pj;
	total_pj -= vars; // calculate the extra PJ without IA and Collision so the total number is the indicated

	GM::Log::log("PARAMS --> Iterations: " + std::to_string(iterations) 
		+ " Vars: " + std::to_string(vars)
		+ " Total PJ: " + std::to_string(old_total_pj_val)
		+ " Total Frames: " + std::to_string(totalFrames)
		+ " Extra PJ: " + std::to_string(total_pj));

	GM::Window_t window{ kSCRWIDTH, kSCRHEIGHT };
	GM::RenderSystem_t render(window);
	GM::GameManager gameManager{ &render };

	GM::ocl_args_d_t oclp{ gpuName, true };
	GM::System_t::ocl = &oclp; // Set the Open CL params before initializing the systems, as they need that set before initializing
	GM::InputSystem_t input(window);
	GM::IASystem_t iaSystem{iterations};
	GM::PhysicsSystem_t physics;
	GM::CollisionSystem_t collision;

	//Should be in order of execution
	//gameManager.addSystem(render);
	gameManager.addSystem(input);
	gameManager.addSystem(iaSystem);
	gameManager.addSystem(physics); // Must be before collision
	gameManager.addSystem(collision); // Must be after physics
	
	//Set key callbacks
	input.upKeyDown = up;
	input.downKeyDown = down;
	input.leftKeyDown = left;
	input.rightKeyDown = right; 
	input.spaceKeyDown = space;
	input.cKeyDown = c;
	input.lKeyDown = l;

	//auto& pattern1 = iaSystem.fm.createPattern(gameManager.entityMan);

	//Formation 1
	//Set formation 1 slots   1023 causes error
	GM::EntityBuilder::buildPattern(gameManager, iaSystem, vars/2, 4, 2, ALABARDERO_PATH, { -5, 1, -50 });
	
	//Formation 2
	GM::EntityBuilder::buildPattern(gameManager, iaSystem, vars/2, 4, 2, ALABARDERO_PATH, { -5, 1, 50 });

	//Formation 3 without ia and collisions
	GM::EntityBuilder::buildPatternWithoutIaColl(gameManager, iaSystem, total_pj/2, 100, 2, ALABARDERO_PATH, { -5, 1, 50 });

	//Formation 4 without ia and collisions
	GM::EntityBuilder::buildPatternWithoutIaColl(gameManager, iaSystem, total_pj/2, 100, 2, ALABARDERO_PATH, { -5, 1, 50 });
	
	/*auto& auxAl = GM::EntityBuilder::buildFullEntity(gameManager, { 1, 1, 1 }, ALABARDERO_PATH, { 1, 1.55f, 0.5f }, { 0, 0.78f, 0 });
	player = &auxAl.getComponent<GM::PhysicsComponent_t>()->position;*/

	/*glm::vec3 cLength0{ 103.75f, 0.75f,  47.32f };
	glm::vec3 cOffset0{ 1.875f, 0.375f, -1.98 };
	ECS::Entity_t& suelo = GM::EntityBuilder::buildFullEntity(gameManager, glm::vec3(-8.5, 0, 0), SUELO_PATH, cLength0, cOffset0);
	auto* sueloPhy = suelo.getComponent<GM::PhysicsComponent_t>();
	sueloPhy->scale.x = 5;
	sueloPhy->scale.z = 2;*/

	//Tower
	/*glm::vec3 cLength2{ 1.25f, 3, 2 };
	glm::vec3 cOffset2{ 0.625f, 1.5f, -1.0f };
	auto& torre = GM::EntityBuilder::buildFullEntity(gameManager, glm::vec3(0, 1, -18.5f), TORRE_PATH, cLength2, cOffset2);
	auto* phy2 = torre.getComponent<GM::PhysicsComponent_t>();
	phy2->gravity = true;
	phy2->scale = { 0.0015f, 0.0015f, 0.0015f };*/

	/*GM::PointLight_t pt({3, 3, -1.5}, { 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f });
	render.lights.push_back(pt);*/
	/*GM::PointLight_t pt2({ -10, 3, -1.5 }, { 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f });
	render.lights.push_back(pt2);*/
	GM::PointLight_t pt3({ -20, 3, -1.5 }, { 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f });
	render.lights.push_back(pt3);
	/*GM::PointLight_t pt4({ 10, 3, -1.5 }, { 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f });
	render.lights.push_back(pt4);*/
	GM::PointLight_t pt5({ 20, 3, -1.5 }, { 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f });
	render.lights.push_back(pt5);
	GM::PointLight_t pt6({ 2, 3.5, -19.5f }, { 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f }); //Tower light
	render.lights.push_back(pt6);

	//GAME LOOP
#ifdef TESTING
	while ((gameManager.update() && totalFrames > 0) /* || GM::Log::getTotalJayaMeasures() <= 1*/) {
		totalFrames--;
	}
#else
	while (gameManager.update());
#endif
	while (!iaSystem.threadDied());
	return 0;
}