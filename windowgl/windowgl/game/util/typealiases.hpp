#pragma once
#include <vector>
#include <iostream>
#include <game/cmp/physics.hpp>
namespace GM {
	//using EntityID_t = std::size_t;
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

	constexpr glm::vec3 RGB_DARKOLIVE{ 0.33f, 0.42f, 0.18f };
	constexpr glm::vec3 RGB_WHITE{ 1.0f, 1.0f, 1.0f };
	constexpr glm::vec3 RGB_GREEN{ 0.0f, 1.0f, 0.0f };
	constexpr glm::vec3 RGB_CORAL{ 1.0f, 0.5f, 0.31f };
}