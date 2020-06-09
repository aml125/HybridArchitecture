#include <game/sys/render.hpp>
#include <glad/glad.h>
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <game/rcmp/shader.hpp>
#include <game/util/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ecs/cmp/entity.hpp>
#include <game\rcmp\mesh.hpp>
#include <game\cmp\physics.hpp>
#include <GLFW/glfw3.h>

namespace GM {

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
};

RenderSystem_t::RenderSystem_t(Window_t window)
	: window{ window }
{
	std::cout << "Starting render system" << std::endl;

	//REGISTRAR CALLBACKS
	//If window size changed, call glviewport
	glfwSetFramebufferSizeCallback(window.window, framebuffer_size_callback);

	//Enable depth buffer
	glEnable(GL_DEPTH_TEST);

	//activate shader
	myShader.init();
	lightShader.init();
	//lightShader.use();
	//myShader.use();

	//View matrix
	view = glm::mat4(1.0f);
	// note that we're translating the scene in the reverse direction of where we want to move
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	//Perspective projection matrix
	projection = glm::perspective(glm::radians(45.0f), window.width / window.height * 1.0f, 0.1f, 100.0f);


	//Send uniforms
	myShader.setInt("texture1", 0);
	myShader.setInt("texture2", 1);
	//myShader.setMatrix4("model", model);
	myShader.setMatrix4("view", view);
	myShader.setMatrix4("projection", projection);

	/*PointLight_t pt(glm::vec3(pointLightPositions[0]), { 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f });
	lights.push_back(pt);*/
	/*PointLight_t pt2(glm::vec3(pointLightPositions[1]), { 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f });
	lights.push_back(pt2);
	PointLight_t pt3(glm::vec3(pointLightPositions[2]), { 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f });
	lights.push_back(pt3);
	PointLight_t pt4(glm::vec3(pointLightPositions[3]), { 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f });
	lights.push_back(pt4);*/

}


RenderSystem_t::~RenderSystem_t() {
	
}

void RenderSystem_t::terminateWindow() {
	glfwTerminate();
}

void RenderSystem_t::update(ECS::EntityManager_t& g) {
	//frame delta time
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	time += deltaTime;

	//Look at matrix
	view = camera.GetViewMatrix();
	projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

	//Move light
	/*lights[0].position.y = sin(glfwGetTime()) + 10;*/

	//RENDERING
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//General shader information
	myShader.setMatrix4("projection", projection);
	myShader.setMatrix4("view", view);
	myShader.setInt("material.diffuse", 0);
	myShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
	myShader.setFloat("material.shininess", 32.0f);
	myShader.setVec3("viewPos", camera.Position);

	drawAllModels(g, g.getComponents<Model_t>());
	drawLights();
	if (drawCollisions) {
		drawCollisionBoxes(g, g.getComponents<BoxCollider_t>());
	}

	glfwSwapBuffers(window.window);
	glfwPollEvents();

	windowShouldClose = glfwWindowShouldClose(window.window);

	if (windowShouldClose) {
		terminateWindow();
	}
}

void RenderSystem_t::setLightInformation() const
{
	//directional light
	myShader.setVec3("dirLight.direction", sunLight.direction);
	myShader.setVec3("dirLight.ambient", sunLight.ambient);
	myShader.setVec3("dirLight.diffuse", sunLight.diffuse);
	myShader.setVec3("dirLight.specular", sunLight.specular);
	myShader.setInt("pointLightsSize", lights.size());

	std::size_t i = 0;
	// point lights
	if (drawPointLights) {
		for (const PointLight_t& pl : lights) {
			myShader.setVec3("pointLights[" + std::to_string(i) + "].position", pl.position);
			myShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", pl.ambient);
			myShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", pl.diffuse);
			myShader.setVec3("pointLights[" + std::to_string(i) + "].specular", pl.specular);
			myShader.setFloat("pointLights[" + std::to_string(i) + "].constant", pl.constantDecrease);
			myShader.setFloat("pointLights[" + std::to_string(i) + "].linear", pl.linearDecrease);
			myShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", pl.quadraticDecrease);
			i++;
		}
	}
}

void RenderSystem_t::drawAllModels(const ECS::EntityManager_t& em, const std::vector<Model_t>& models) const {
	myShader.use();
	myShader.setMatrix4("projection", projection);
	myShader.setMatrix4("view", view);
	myShader.setFloat("material.shininess", 32);
	for (const auto& mod : models) {
		for (const Mesh_t& m : mod.getMeshes()) {
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			for (std::size_t i = 0; i < m.textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
				// retrieve texture number (the N in diffuse_textureN)
				std::string number;
				std::string name = m.textures[i].type;
				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++);

				std::string finalName = ("material." + name + number);
				myShader.setFloat(finalName.c_str(), i);
				glBindTexture(GL_TEXTURE_2D, m.textures[i].id);
			}
			glActiveTexture(GL_TEXTURE0);

			// draw mesh
			glBindVertexArray(m.VAO);
			glm::mat4 model = glm::mat4(1.0f);
			auto* phy = em.getEntity(mod.entityID).getComponent<PhysicsComponent_t>();
			if (phy == nullptr) {
				std::cout << "render.cpp ERROR to draw a model its entity needs a PhysicsComponent_t\n";
				exit(-1);
			}

			if (phy->rotation.x != 0 || phy->rotation.y != 0 || phy->rotation.z != 0) {
				model = glm::rotate(model, glm::length(phy->rotation), glm::normalize(phy->rotation)); //Rotate only if there is rotation, because if all are 0 it gliches
			}
			model = glm::translate(model, phy->position);
			model = glm::scale(model, phy->scale);
			myShader.setMatrix4("model", model);
			
			setLightInformation();
			glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

		}
	}
}

void RenderSystem_t::drawLights() const
{
	for (const PointLight_t& pl : lights) {
		drawLightSource(pl);
	}
}

void RenderSystem_t::drawCollisionBoxes(const ECS::EntityManager_t& em, const std::vector<BoxCollider_t>& boxes) const
{
	for (const auto& bc : boxes) {
		auto* phy = em.getEntity(bc.entityID).getComponent<PhysicsComponent_t>();
		drawCollisionBox(bc, phy->position);
	}
}

void RenderSystem_t::drawCollisionBox(const BoxCollider_t& box, const glm::vec3 position) const
{
	glm::vec3 auxPos{ position.x+box.offset.x, position.y+box.offset.y, position.z+box.offset.z };
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	lightShader.use();
	lightShader.setMatrix4("projection", projection);
	lightShader.setMatrix4("view", view);
	lightShader.setVec3("objectColor", RGB_GREEN);
	lightShader.setVec3("lightColor", RGB_GREEN);

	glBindVertexArray(collisionRenderer.VAO);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, auxPos);
	model = glm::scale(model, glm::vec3(box.length));
	lightShader.setMatrix4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RenderSystem_t::drawLightSource(const PointLight_t& light) const
{
	lightShader.use();
	lightShader.setMatrix4("projection", projection);
	lightShader.setMatrix4("view", view);
	lightShader.setVec3("objectColor", light.diffuse);
	lightShader.setVec3("lightColor", light.diffuse);

	glBindVertexArray(light.boxRenderer.VAO);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, light.position);
	model = glm::scale(model, glm::vec3(0.2f));
	lightShader.setMatrix4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}
}