#include <sys/render.hpp>
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
#include <util/gamecontext.hpp>
#include <util/typealiases.hpp>
#include <cmp/entity.hpp>


namespace ECS {
	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
};

void RenderSystem_t::mouse_move(GLFWwindow* window)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	if (firstMouse) // this bool variable is initially set to true
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	//Get the distance of the mouse compared to last
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;


	yaw += xoffset;
	pitch += yoffset;


	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	//Calculate direction vector, using the cos and sin function to calculate de pitch and yaw
	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	camera.Front = glm::normalize(front);
};

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (RenderSystem_t::fov >= 1.0f && RenderSystem_t::fov <= 45.0f)
		RenderSystem_t::fov -= yoffset;
	if (RenderSystem_t::fov <= 1.0f)
		RenderSystem_t::fov = 1.0f;
	if (RenderSystem_t::fov >= 45.0f)
		RenderSystem_t::fov = 45.0f;
};

void processInput(GLFWwindow* window)
{
	float cameraSpeed = 2.5f * RenderSystem_t::deltaTime; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		/*RenderSystem_t::camera.Position += cameraSpeed * RenderSystem_t::camera.Front;*/
		RenderSystem_t::camera.ProcessKeyboard(Camera_Movement::FORWARD, RenderSystem_t::deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		//RenderSystem_t::camera.Position -= cameraSpeed * RenderSystem_t::camera.Front;
		RenderSystem_t::camera.ProcessKeyboard(Camera_Movement::BACKWARD, RenderSystem_t::deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		//RenderSystem_t::camera.Position -= glm::normalize(glm::cross(RenderSystem_t::camera.Front, RenderSystem_t::camera.Up)) * cameraSpeed;
		RenderSystem_t::camera.ProcessKeyboard(Camera_Movement::LEFT, RenderSystem_t::deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		//RenderSystem_t::camera.Position += glm::normalize(glm::cross(RenderSystem_t::camera.Front, RenderSystem_t::camera.Up)) * cameraSpeed;
		RenderSystem_t::camera.ProcessKeyboard(Camera_Movement::RIGHT, RenderSystem_t::deltaTime);
};

void RenderSystem_t::init() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//MAC LOOSERS glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//Create window
	window = glfwCreateWindow(m_w, m_h,
		"Primera ventana", NULL, NULL); //Resolution and name
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		char c = getchar();
		return;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Initialize glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		char c = getchar();
		return;
	}

	//Tell glad windows size (1, 2, 3, 4) where 1, 2 is the left corner
	glViewport(0, 0, m_w, m_h);
}

RenderSystem_t::RenderSystem_t(unsigned int w, unsigned int h) 
    : m_w{w}, m_h{h}
{
	std::cout << "Starting render system" << std::endl;

	init();


	//REGISTRAR CALLBACKS
	//If window size changed, call glviewport
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//Enable depth buffer
	glEnable(GL_DEPTH_TEST);

	//activate shader
	myShader.init();
	lightShader.init();
	//lightShader.use();
	//myShader.use();

	//Model matrix
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	//View matrix
	view = glm::mat4(1.0f);
	// note that we're translating the scene in the reverse direction of where we want to move
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	//Perspective projection matrix
	projection = glm::perspective(glm::radians(45.0f), m_w / m_h * 1.0f, 0.1f, 100.0f);


	//Send uniforms
	myShader.setInt("texture1", 0);
	myShader.setInt("texture2", 1);
	//myShader.setMatrix4("model", model);
	myShader.setMatrix4("view", view);
	myShader.setMatrix4("projection", projection);

	PointLight_t pt(glm::vec3(pointLightPositions[0]), { 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f });
	lights.push_back(pt);
	/*PointLight_t pt2(glm::vec3(pointLightPositions[1]), { 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f });
	lights.push_back(pt2);
	PointLight_t pt3(glm::vec3(pointLightPositions[2]), { 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f });
	lights.push_back(pt3);
	PointLight_t pt4(glm::vec3(pointLightPositions[3]), { 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f });
	lights.push_back(pt4);*/

}


RenderSystem_t::~RenderSystem_t() {
	glfwTerminate();
}

bool RenderSystem_t::update(const GameContext_t& g) {
	//frame delta time
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	//INPUT
	processInput(window);
	mouse_move(window);

	//Look at matrix
	view = camera.GetViewMatrix();
	projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

	//Move light
	lights[0].position.y = sin(glfwGetTime()) + 10;

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

	drawLights();
	drawAllEntities(g.getEntities());

	glfwSwapBuffers(window);
	glfwPollEvents();
    
	return !glfwWindowShouldClose(window);
}

//DELETE THIS
glm::vec3 cubePositions[] = {
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
	for (const PointLight_t& pl : lights) {
		myShader.setVec3("pointLights["+std::to_string(i)+"].position", pl.position);
		myShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", pl.ambient);
		myShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", pl.diffuse);
		myShader.setVec3("pointLights[" + std::to_string(i) + "].specular", pl.specular);
		myShader.setFloat("pointLights[" + std::to_string(i) + "].constant", pl.constantDecrease);
		myShader.setFloat("pointLights[" + std::to_string(i) + "].linear", pl.linearDecrease);
		myShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", pl.quadraticDecrease);
		i++;
	}
}

void RenderSystem_t::drawAllEntities(const VecEntities_t& entities) const {
	myShader.use();
	myShader.setMatrix4("projection", projection);
	myShader.setMatrix4("view", view);
	myShader.setFloat("material.shininess", 32);
	for (const Entity_t& e : entities) {
		for (const Mesh_t& m : e.getModel().getMeshes()) {
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
			model = glm::translate(model, e.phy->position);
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