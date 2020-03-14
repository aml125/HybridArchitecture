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

	lights.emplace_back(glm::vec3(pointLightPositions[0]), RGB_WHITE );
	lights.emplace_back(glm::vec3(5.0f, 5.0f, 5.0f), RGB_CORAL);
	lights.emplace_back(glm::vec3(pointLightPositions[1]), RGB_WHITE );
	lights.emplace_back(glm::vec3(pointLightPositions[2]), RGB_WHITE );
	lights.emplace_back(glm::vec3(pointLightPositions[3]), RGB_WHITE );

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
	drawLightSource(lights[2]);
	drawLightSource(lights[3]);
	drawLightSource(lights[4]);
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
	// directional light
	myShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	myShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	myShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	myShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
	// point light 1
	myShader.setVec3("pointLights[0].position", pointLightPositions[0]);
	myShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	myShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	myShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	myShader.setFloat("pointLights[0].constant", 1.0f);
	myShader.setFloat("pointLights[0].linear", 0.09);
	myShader.setFloat("pointLights[0].quadratic", 0.032);
	// point light 2
	myShader.setVec3("pointLights[1].position", pointLightPositions[1]);
	myShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	myShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	myShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	myShader.setFloat("pointLights[1].constant", 1.0f);
	myShader.setFloat("pointLights[1].linear", 0.09);
	myShader.setFloat("pointLights[1].quadratic", 0.032);
	// point light 3
	myShader.setVec3("pointLights[2].position", pointLightPositions[2]);
	myShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	myShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	myShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	myShader.setFloat("pointLights[2].constant", 1.0f);
	myShader.setFloat("pointLights[2].linear", 0.09);
	myShader.setFloat("pointLights[2].quadratic", 0.032);
	// point light 4
	myShader.setVec3("pointLights[3].position", pointLightPositions[3]);
	myShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	myShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	myShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	myShader.setFloat("pointLights[3].constant", 1.0f);
	myShader.setFloat("pointLights[3].linear", 0.09);
	myShader.setFloat("pointLights[3].quadratic", 0.032);
	//myShader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);

	glBindVertexArray(lights[1].VAO);
	glm::mat4 model = glm::mat4(1.0f);
	lightShader.setMatrix4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	for (unsigned int i = 0; i < 10; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		myShader.setMatrix4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

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