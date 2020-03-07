#pragma once
#include <vector>
#include <iostream>
#include <cmp/entity.hpp>
#include <cmp\physics.hpp>
namespace ECS {
    using VecEntities_t = std::vector<Entity_t>;
    using VecPhysics_t = std::vector<PhysicsComponent_t>;
	/*using VecMeshes_t = std::vector<Mesh_t>;
	using VecTextures_t = std::vector<Texture_t>;
	using VecVertex_t = std::vector<Vertex_t>;*/

	enum Camera_Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};
}