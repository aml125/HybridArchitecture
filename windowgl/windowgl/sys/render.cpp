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

RenderSystem_t::RenderSystem_t(uint32_t w, uint32_t h) 
    : m_w{w}, m_h{h}
{
    std::cout << "Starting render system" << std::endl;
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

	//REGISTRAR CALLBACKS
	//If window size changed, call glviewport
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//Enable depth buffer
	glEnable(GL_DEPTH_TEST);

	//activate shader
	myShader.init();
	myShader.use();

	////Model matrix
	//model = glm::mat4(1.0f);
	//model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

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
	myShader.setMatrix4("projection", projection);
	myShader.setMatrix4("view", view);

	//RENDERING
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawAllEntities(g.getEntities());

	glfwSwapBuffers(window);
	glfwPollEvents();
    
	return !glfwWindowShouldClose(window);
}

void RenderSystem_t::drawAllEntities(const VecEntities_t& entities) const {
	for (Entity_t e : entities) {
		// activate the texture unit first before binding texture
		glActiveTexture(GL_TEXTURE0);
		//bind texture
		glBindTexture(GL_TEXTURE_2D, e.texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, e.texture2);

		//transform object
		/*glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, (float)(glfwGetTime() * glm::radians(50.0f)), glm::vec3(0.5f, 1.0f, 0.0f));
		myShader.setMatrix4("model", model);*/
		////bind vao
		//glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//bind vao AND DRAW
		glBindVertexArray(e.VAO);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, e.position);
		model = glm::rotate(model, glm::radians(e.angle), glm::vec3(1.0f, 0.3f, 0.5f));
		myShader.setMatrix4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

}