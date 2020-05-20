#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace GM {
	struct Window_t {
		Window_t(unsigned int width, unsigned int heigth);

		const unsigned int width{ 0 }, height{ 0 };
		GLFWwindow* window;

	private:
		void init(int width, int heigth);
	};
}