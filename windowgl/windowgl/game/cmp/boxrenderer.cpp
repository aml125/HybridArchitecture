#include <game/cmp/boxrenderer.hpp>
#include <glad\glad.h>

namespace GM {
	BoxRenderer_t::BoxRenderer_t() {
		bindVertexArray();
	}

	void BoxRenderer_t::bindVertexArray()
	{
		//Create and bind Vertex buffer (VBO)
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//Generate VAO object. Vertex Array Object
		glGenVertexArrays(1, &VAO);
		// 1. bind Vertex Array Object
		glBindVertexArray(VAO);
		// 2. copy our vertices array in a buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//set our vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}
}