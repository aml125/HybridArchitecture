#pragma once
#include <game\rcmp\window.hpp>

namespace GM {
	struct InputSystem_t {
		void processInput(GLFWwindow* window);
		InputSystem_t(Window_t& window);
		void mouse_move(GLFWwindow* window);
		bool update();

		bool firstMouse = true;
		double lastX = 400, lastY = 300;
		Window_t& window;

		//Function pointers
		void (*upKeyDown)() = nullptr;
		void (*downKeyDown)() = nullptr;
		void (*leftKeyDown)() = nullptr;
		void (*rightKeyDown)() = nullptr;
		void (*spaceKeyDown)() = nullptr;
		void (*cKeyDown)() = nullptr;
		void (*lKeyDown)() = nullptr;
	};
}