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
#include <game\util\TimeMeasure.hpp>
#include <game\util\log.hpp>
//#define TIMEMEASURE

namespace GM {

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
};

RenderSystem_t::RenderSystem_t(Window_t window)
	: window{ window }
{
	GM::Log::log("Starting render system");

	//REGISTRAR CALLBACKS
	//If window size changed, call glviewport
	glfwSetFramebufferSizeCallback(window.window, framebuffer_size_callback);

	//Enable depth buffer
	glEnable(GL_DEPTH_TEST);

	//activate shader
	myShader.init();
	lightShader.init();
	instancingShader.init();
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

	flag.loadModel("assets\\models\\bandera\\bandera.obj");

	//Create instance buffer
	glGenBuffers(1, &instanceVBO);
}


RenderSystem_t::~RenderSystem_t() {
	//Free memory
	if (modelMatrixArray != NULL) {
		delete(modelMatrixArray);
	}
}

void RenderSystem_t::terminateWindow() {
	glfwTerminate();
}

void RenderSystem_t::update(ECS::EntityManager_t& g) {
	#ifdef TIMEMEASURE
	 //Log::log("Non graphic computing: " + std::to_string(tm.GetCounter()));
	 tm.StartCounter();
	#endif
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
	drawAllInstantiatedModels(g, g.getComponents<InstantiatedModel_t>());
	//drawFormationTargets(g.getComponents<IA_t>());
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

#ifdef TIMEMEASURE
	Log::log("Graphic computing: " + std::to_string(tm.GetCounter()));
	//tm.StartCounter();
#endif
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
				Log::log("render.cpp ERROR to draw a model its entity needs a PhysicsComponent_t");
				exit(-1);
			}

			model = glm::translate(model, phy->position);
			model = glm::scale(model, phy->scale);
			if (phy->rotation.x != 0 || phy->rotation.y != 0 || phy->rotation.z != 0) {
				model = glm::rotate(model, glm::length(phy->rotation), glm::normalize(phy->rotation)); //Rotate only if there is rotation, because if all are 0 it gliches
			}
			myShader.setMatrix4("model", model);
			
			setLightInformation();
			glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

		}
	}
}

void RenderSystem_t::drawAllInstantiatedModels(const ECS::EntityManager_t& em, const std::vector<InstantiatedModel_t>& models) {
	instancingShader.use();
	instancingShader.setMatrix4("projection", projection);
	instancingShader.setMatrix4("view", view);
	instancingShader.setFloat("material.shininess", 32);

	//Check if number of models changed
	bool changed = false;

	if (totalInstances != models.size()) {
		changed = true;
		totalInstances = models.size();
	}

	if (changed) {
		if (models.size() > 0) {
			modelMatrixArray = new glm::mat4[models.size()];
		}
	}

	int i = 0;

	for (const auto& mod : models) {

		glm::mat4 model = glm::mat4(1.0f);
		auto* phy = em.getEntity(mod.entityID).getComponent<PhysicsComponent_t>();
		if (phy == nullptr) {
			Log::log("render.cpp ERROR to draw a model its entity needs a PhysicsComponent_t");
			exit(-1);
		}

		model = glm::translate(model, phy->position);
		model = glm::scale(model, phy->scale);
		if (phy->rotation.x != 0 || phy->rotation.y != 0 || phy->rotation.z != 0) {
			model = glm::rotate(model, glm::length(phy->rotation), glm::normalize(phy->rotation)); //Rotate only if there is rotation, because if all are 0 it gliches
		}

		modelMatrixArray[i++] = model;
	}

	//Draws one model multiple times. Only works if there is only one model to use with instancing. FAST WORKAROUND
	if (models.size() > 0) {
		for (int q = 0; q < models[0].getMeshes().size(); q++) {
			auto& mesh = models[0].getMeshes()[q];

			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			for (std::size_t i = 0; i < mesh.textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
				// retrieve texture number (the N in diffuse_textureN)
				std::string number;
				std::string name = mesh.textures[i].type;
				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++);

				std::string finalName = ("material." + name + number);  // Set the textures using the naming convention texture_(diffuse/specular)+i. See "Fragment.glsl"
				instancingShader.setFloat(finalName.c_str(), i);
				glBindTexture(GL_TEXTURE_2D, mesh.textures[i].id);
			}
			glActiveTexture(GL_TEXTURE0);


			if (changed) {
				glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * models.size(), modelMatrixArray, GL_STATIC_DRAW);

				glBindVertexArray(mesh.VAO);

				//glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)0);
				glEnableVertexAttribArray(4);
				glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(sizeof(glm::vec4) * 1));
				glEnableVertexAttribArray(5);
				glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(sizeof(glm::vec4) * 2));
				glEnableVertexAttribArray(6);
				glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(sizeof(glm::vec4) * 3));

				glVertexAttribDivisor(3, 1); // tell OpenGL this is an instanced vertex attribute.
				glVertexAttribDivisor(4, 1); // tell OpenGL this is an instanced vertex attribute.
				glVertexAttribDivisor(5, 1); // tell OpenGL this is an instanced vertex attribute.
				glVertexAttribDivisor(6, 1); // tell OpenGL this is an instanced vertex attribute.

				glBindVertexArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
			else {
				glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * models.size(), modelMatrixArray);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

			glBindVertexArray(mesh.VAO);
			setLightInformation();
			glDrawElementsInstanced(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0, models.size());
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

void RenderSystem_t::drawFormationTargets(const std::vector<IA_t> ias) {
	for (auto& ia : ias) {
		if (ia.isAnchorPoint) {
			for (const Mesh_t& m : flag.getMeshes()) {
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

				model = glm::translate(model, ia.target.position);
				model = glm::scale(model, { 0.01, 0.01, 0.01 });
				if (ia.target.orientation.x != 0 || ia.target.orientation.y != 0 || ia.target.orientation.z != 0) {
					model = glm::rotate(model, glm::length(ia.target.orientation), glm::normalize(ia.target.orientation)); //Rotate only if there is rotation, because if all are 0 it gliches
				}
				myShader.setMatrix4("model", model);

				setLightInformation();
				glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);

			}
		}
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
	lightShader.setVec3("lightColor", RGB_GREEN);

	glBindVertexArray(light.boxRenderer.VAO);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, light.position);
	model = glm::scale(model, glm::vec3(0.2f));
	lightShader.setMatrix4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}
}