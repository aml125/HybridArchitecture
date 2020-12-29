#include <game/rcmp/window.hpp>
#include <iostream>
#include <game\util\log.hpp>

namespace GM {
	Window_t::Window_t(unsigned int w, unsigned int h) 
		: width{w}, height{h}
	{
		init(w, h);
	}

	void Window_t::init(int m_w, int m_h) {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		//MAC LOOSERS glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		//Create window
		window = glfwCreateWindow(m_w, m_h,
			"Prototipo", NULL, NULL); //Resolution and name
		if (window == NULL)
		{
			GM::Log::log("Failed to create GLFW window");
			glfwTerminate();
			char c = getchar();
			return;
		}
		glfwMakeContextCurrent(window);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		//Initialize glad
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			GM::Log::log("Failed to initialize GLAD");
			char c = getchar();
			return;
		}

		//Tell glad windows size (1, 2, 3, 4) where 1, 2 is the left corner
		glViewport(0, 0, m_w, m_h);
	}
}