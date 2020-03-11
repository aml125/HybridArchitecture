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

RenderSystem_t::RenderSystem_t(uint32_t w, uint32_t h) 
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
	//myShader.use();

	////Model matrix
	//model = glm::mat4(1.0f);
	//model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	//View matrix
	view = glm::mat4(1.0f);
	// note that we're translating the scene in the reverse direction of where we want to move
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	//Perspective projection matrix
	projection = glm::perspective(glm::radians(45.0f), m_w / m_h * 1.0f, 0.1f, 100.0f);


	////Send uniforms
	//myShader.setInt("texture1", 0);
	//myShader.setInt("texture2", 1);
	////myShader.setMatrix4("model", model);
	//myShader.setMatrix4("view", view);
	//myShader.setMatrix4("projection", projection);

	lights.emplace_back(glm::vec3(3.0f, 1.0f, 0.0f), RGB_WHITE );
	lights.emplace_back(glm::vec3(5.0f, 5.0f, 5.0f), RGB_CORAL);
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
	lights[0].position.y = sin(glfwGetTime());

	//RENDERING
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawLightSource(lights[0]);
	drawAllEntities(g.getEntities());

	glfwSwapBuffers(window);
	glfwPollEvents();
    
	return !glfwWindowShouldClose(window);
}

void RenderSystem_t::drawAllEntities(const VecEntities_t& entities) const {
	myShader.use();
	myShader.setMatrix4("projection", projection);
	myShader.setMatrix4("view", view);
	for (const Entity_t& e : entities) {
		for (const Mesh_t& m : e.getModel().getMeshes()) {
			//unsigned int diffuseNr = 1;
			//unsigned int specularNr = 1;
			//for (unsigned int i = 0; i < m.textures.size(); i++)
			//{
			//	glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
			//	// retrieve texture number (the N in diffuse_textureN)
			//	std::string number;
			//	std::string name = m.textures[i].type;
			//	if (name == "texture_diffuse")
			//		number = std::to_string(diffuseNr++);
			//	else if (name == "texture_specular")
			//		number = std::to_string(specularNr++);

			//	myShader.setFloat(("material." + name + number).c_str(), i);
			//	glBindTexture(GL_TEXTURE_2D, m.textures[i].id);
			//}
			//glActiveTexture(GL_TEXTURE0);

			//// draw mesh
			//glBindVertexArray(m.VAO);
			//glm::mat4 model = glm::mat4(1.0f);
			//myShader.setMatrix4("model", model);
			//
			//myShader.setVec3("objectColor", RGB_CORAL);
			//myShader.setVec3("lightColor", lights[0].color);
			//myShader.setFloat("ambientStrength", lights[0].ambientStrength);
			//glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, 0);
			//glBindVertexArray(0);

		}
	}

	//DELETE THIS
	myShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lights[1].textureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lights[1].specularID);

	glm::vec3 diffuseColor = lights[0].color * glm::vec3(0.5f);
	glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

	myShader.setMatrix4("projection", projection);
	myShader.setMatrix4("view", view);
	myShader.setInt("material.diffuse", 0);
	myShader.setInt("material.specular", 1);
	myShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
	myShader.setFloat("material.shininess", 32.0f);
	myShader.setVec3("viewPos", camera.Position);
	myShader.setVec3("light.ambient", ambientColor);
	myShader.setVec3("light.diffuse", diffuseColor); // darken diffuse light a bit
	myShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
	myShader.setVec3("light.position", lights[0].position);

	glBindVertexArray(lights[1].VAO);
	glm::mat4 model = glm::mat4(1.0f);
	lightShader.setMatrix4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

}

void RenderSystem_t::drawLightSource(const LightSource_t& light) const
{
	lightShader.use();
	lightShader.setMatrix4("projection", projection);
	lightShader.setMatrix4("view", view);
	lightShader.setVec3("objectColor", light.color);
	lightShader.setVec3("lightColor", light.color);

	glBindVertexArray(light.VAO);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, light.position);
	model = glm::scale(model, glm::vec3(0.2f));
	lightShader.setMatrix4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//glBindVertexArray(light.VAO);
	//model = glm::mat4(1.0f);
	////glm::scale(model, glm::vec3(0.2f));
	//glm::translate(model, { 8.0, 8.0, 8.0 });
	//myShader.setMatrix4("projection", projection);
	//myShader.setMatrix4("view", view);
	//myShader.setMatrix4("model", model);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
}

}