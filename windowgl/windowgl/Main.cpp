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

constexpr uint32_t kSCRWIDTH{ 1024 };
constexpr uint32_t kSCRHEIGHT{ 768 };

int main()
{
	ECS::RenderSystem_t Render(kSCRWIDTH, kSCRHEIGHT);
	ECS::EntityManager_t EntityMan;
	for (size_t i = 0; i < 10; i++)
	{
		EntityMan.createEntity(cubePositions[i]);
	}
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