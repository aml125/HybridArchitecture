#pragma once

namespace ECS {
	struct InputSystem_t {
		void processInput(GLFWwindow* window);
		InputSystem_t(Window_t& window);
		void mouse_move(GLFWwindow* window);
		bool update();

		bool firstMouse = true;
		double lastX = 400, lastY = 300;
		Window_t& window;

		//Function pointers
		void (*upKeyDown)();
		void (*downKeyDown)();
		void (*leftKeyDown)();
		void (*rightKeyDown)();
		void (*spaceKeyDown)();
		void (*cKeyDown)();
	};
}