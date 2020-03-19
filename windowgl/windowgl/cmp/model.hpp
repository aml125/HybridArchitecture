#pragma once
#include <Shader.h>
//#include <cmp\mesh.hpp>
#include <assimp\scene.h>
#include <vector>
#include <iostream>
#include <cmp/mesh.hpp>

namespace ECS {
	struct Model_t
	{
		/*  Functions   */
		Model_t(const char* path)
			: meshes{}, directory{}
		{
			loadModel(path);
		};
		const std::vector<Mesh_t>& getMeshes() const;

		inline static std::vector<Texture_t> texturesLoaded{};
	private:
		/*  Model Data  */
		std::vector<Mesh_t> meshes;
		std::string directory;

		/*  Functions   */
		void loadModel(std::string path);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh_t processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture_t> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
			const std::string& typeName);
		unsigned int textureFromFile(const char* path, const std::string& directory);
		bool textureIsPressent(const std::string& path, Texture_t& textureFound);
	};
}