#pragma once
#include <cmp\vertex.hpp>
#include <vector>
//#include <Shader.h>
#include <cmp\texture.hpp>
//#include <util\typealiases.hpp>

namespace ECS {
	struct Mesh_t {
		/*  Mesh Data  */
		std::vector<Vertex_t> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture_t> textures;

		/*  Functions  */
		Mesh_t(std::vector<Vertex_t> vertices, std::vector<unsigned int> indices, std::vector<Texture_t> textures);

		/*  Render data  */
		unsigned int VAO = 0;
		unsigned int VBO = 0; 
		unsigned int EBO = 0;

	private:
		/*  Functions    */
		void setupMesh();
	};
}