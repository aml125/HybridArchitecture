#include <game/sys/render.hpp>
#include <game/sys/input.hpp>

namespace GM {
void InputSystem_t::mouse_move(GLFWwindow* window)
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


	RenderSystem_t::yaw += xoffset;
	RenderSystem_t::pitch += yoffset;


	if (RenderSystem_t::pitch > 89.0f)
		RenderSystem_t::pitch = 89.0f;
	if (RenderSystem_t::pitch < -89.0f)
		RenderSystem_t::pitch = -89.0f;

	//Calculate direction vector, using the cos and sin function to calculate de pitch and yaw
	glm::vec3 front;
	front.x = cos(glm::radians(RenderSystem_t::pitch)) * cos(glm::radians(RenderSystem_t::yaw));
	front.y = sin(glm::radians(RenderSystem_t::pitch));
	front.z = cos(glm::radians(RenderSystem_t::pitch)) * sin(glm::radians(RenderSystem_t::yaw));
	RenderSystem_t::camera.Front = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (RenderSystem_t::fov >= 1.0f && RenderSystem_t::fov <= 45.0f)
		RenderSystem_t::fov -= yoffset;
	if (RenderSystem_t::fov <= 1.0f)
		RenderSystem_t::fov = 1.0f;
	if (RenderSystem_t::fov >= 45.0f)
		RenderSystem_t::fov = 45.0f;
};

void InputSystem_t::processInput(GLFWwindow* window)
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

	//Input events
	if ((upKeyDown != nullptr) && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		upKeyDown();
	}

	if ((downKeyDown != nullptr) && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		downKeyDown();
	}

	if ((leftKeyDown != nullptr) && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		leftKeyDown();
	}

	if ((rightKeyDown != nullptr) && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		rightKeyDown();
	}

	if ((spaceKeyDown != nullptr) && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		spaceKeyDown();
	}

	if ((cKeyDown != nullptr) && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		cKeyDown();
	}

	if ((lKeyDown != nullptr) && glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		lKeyDown();
	}
};

InputSystem_t::InputSystem_t(Window_t& win)
	: window{win}
{
	glfwSetScrollCallback(window.window, scroll_callback);
}

void InputSystem_t::update(ECS::EntityManager_t& em)
{
	//INPUT
	processInput(window.window);
	mouse_move(window.window);
}
}