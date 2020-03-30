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

const std::string MESA_PATH = "C:\\Users\\tonet\\Desktop\\OpenGl\\windowgl\\x64\\Debug\\models\\mesa\\mesa.fbx";
const std::string NANOSUIT_PATH = "C:\\Users\\tonet\\Desktop\\OpenGl\\windowgl\\x64\\Debug\\models\\nanosuit\\nanosuit.obj";
const std::string DICE_PATH = "C:\\Users\\tonet\\Desktop\\OpenGl\\windowgl\\x64\\Debug\\models\\dice\\dice.fbx";

int main()
{
	ECS::Window_t window{ kSCRWIDTH, kSCRHEIGHT };
	ECS::RenderSystem_t Render(window);
	ECS::EntityManager_t EntityMan;
	
	EntityMan.createEntity(glm::vec3(0, 0, 0), MESA_PATH);
	EntityMan.createEntity(glm::vec3(10, 0, 0), NANOSUIT_PATH);

	ECS::VecEntities_t& vec = EntityMan.getEntities();
	vec[0].collider.length.x = vec[0].collider.length.y = vec[0].collider.length.z = 2;
	vec[0].phy->speed.x = 0.2f;
	
	vec[1].collider.offset.y = 7.5;
	vec[1].collider.length.x = 4;
	vec[1].collider.length.y = 15;
	vec[1].collider.length.z = 1;
	vec[1].phy->speed.x = -0.2f;

	ECS::PhysicsSystem_t Physics;
	ECS::CollisionSystem_t Collision;
	while (Render.update(EntityMan)) {
		Physics.update(EntityMan);
		Collision.update(EntityMan);
	}
	Render.~RenderSystem_t();
	char c = getchar();
	return 0;
}