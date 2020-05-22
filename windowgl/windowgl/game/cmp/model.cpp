#include <game/cmp/model.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <game/util/stb_image.h>

namespace GM {
	const std::vector<Mesh_t>& Model_t::getMeshes() const
	{
		return models[path].getMeshes();
	}

	void Model_t::loadModel(const std::string& path)
	{
		this->path = path;
		auto it = models.find(path);
		if (it == models.end()) {
			LoadedModel_t newModel;
			auto* ptr = &newModel;
			models[path] = newModel;
			models[path].loadModel(path);
		}
	}
}