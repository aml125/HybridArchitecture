#include <game/rcmp/loadedmodel.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <game/util/stb_image.h>
#include <game\util\log.hpp>

namespace GM {
	const std::vector<Mesh_t>& LoadedModel_t::getMeshes() const
	{
		return meshes;
	}

	void LoadedModel_t::loadModel(const std::string& path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			GM::Log::log("ERROR::ASSIMP::" + std::string(importer.GetErrorString()));
			return;
		}
		auto end = path.find_last_of('\\');
		directory = path.substr(0, end);

		processNode(scene->mRootNode, scene);
	}

	void LoadedModel_t::processNode(aiNode* node, const aiScene* scene)
	{
		// process all the node's meshes (if any)
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		// then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh_t LoadedModel_t::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex_t> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture_t> textures;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex_t vertex;

			// process vertex positions, normals and texture coordinates
			glm::vec3 vector;
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;

			vector = glm::vec3{};
			if (mesh->mNormals != nullptr) {
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
			}
			vertex.Normal = vector;

			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			vertices.push_back(vertex);
		}
		// process indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// process material
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<Texture_t> diffuseMaps = loadMaterialTextures(material,
				aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			std::vector<Texture_t> specularMaps = loadMaterialTextures(material,
				aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		return Mesh_t(vertices, indices, textures);
	}

	std::vector<Texture_t> LoadedModel_t::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName)
	{
		std::vector<Texture_t> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString path;
			mat->GetTexture(type, i, &path);

			Texture_t textureFound;
			if (textureIsPressent(path.C_Str(), textureFound)) {
				textures.push_back(textureFound);
			}
			else {
				Texture_t texture;
				texture.id = textureFromFile(path.C_Str(), directory);
				texture.type = typeName;
				texture.path = path.C_Str();
				textures.push_back(texture);
			}
		}
		return textures;
	}

	unsigned int LoadedModel_t::textureFromFile(const char* path, const std::string& directory)
	{
		std::string filename = std::string(path);
		filename = directory + '/' + filename;
		Log::log("Searching for texture: " + filename);

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format = GL_RGB; //Initialized for nothing
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			Log::log("Texture failed to load at path: " + std::string(path));
			stbi_image_free(data);
		}

		return textureID;
	}

	bool LoadedModel_t::textureIsPressent(const std::string& path, Texture_t& foundTexture)
	{
		for (Texture_t& tex : texturesLoaded) {
			if (tex.path == path) {
				foundTexture = tex;
				return true;
			}
		}
		return false;
	}
}