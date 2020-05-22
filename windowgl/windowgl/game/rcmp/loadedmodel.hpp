#pragma once
#include <game/rcmp/shader.hpp>
//#include <cmp\mesh.hpp>
#include <assimp\scene.h>
#include <vector>
#include <iostream>
#include <game/rcmp/mesh.hpp>
#include <ecs/cmp/component.hpp>

namespace GM {
	struct LoadedModel_t
	{
		/*  Functions   */
		LoadedModel_t()
			: meshes{}
		{
		};
		const std::vector<Mesh_t>& getMeshes() const;
		void loadModel(const std::string& path);

		inline static std::vector<Texture_t> texturesLoaded{};
	private:
		/*  Model Data  */
		std::vector<Mesh_t> meshes;
		std::string directory{};

		/*  Functions   */
		void processNode(aiNode* node, const aiScene* scene);
		Mesh_t processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture_t> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
			const std::string& typeName);
		unsigned int textureFromFile(const char* path, const std::string& directory);
		bool textureIsPressent(const std::string& path, Texture_t& textureFound);
	};
}