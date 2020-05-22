#pragma once
#include <game/rcmp/shader.hpp>
//#include <cmp\mesh.hpp>
#include <assimp\scene.h>
#include <vector>
#include <iostream>
#include <game/rcmp/mesh.hpp>
#include <ecs/cmp/component.hpp>
#include <game\rcmp\loadedmodel.hpp>
#include <unordered_map>

namespace GM {
	struct Model_t : ECS::Component_t
	{
		/*  Functions   */
		Model_t(std::size_t eid)
			: Component_t{eid}
		{
		};
		const std::vector<Mesh_t>& getMeshes() const;
		void loadModel(const std::string& path);

	private:
		inline static std::unordered_map<std::string, LoadedModel_t> models{};
		std::string path{};
	};
}