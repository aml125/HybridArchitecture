#include <cmp/directionalLight.hpp>

namespace ECS {
	DirectionalLight_t::DirectionalLight_t(const glm::vec3& dir, 
		const glm::vec3& amb, const glm::vec3& diff, const glm::vec3& spec)
		: direction{ dir }, ambient{ amb }, diffuse{ diff }, specular{ spec }
	{
		////Create and bind Vertex buffer (VBO)
		//glGenBuffers(1, &VBO);
		//glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		////Generate VAO object. Vertex Array Object
		//glGenVertexArrays(1, &VAO);
		//// 1. bind Vertex Array Object
		//glBindVertexArray(VAO);
		//// 2. copy our vertices array in a buffer for OpenGL to use
		//glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		////set our vertex attributes pointers
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		//glEnableVertexAttribArray(1);
		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		//glEnableVertexAttribArray(2);
	}
}