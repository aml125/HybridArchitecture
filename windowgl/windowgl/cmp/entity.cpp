#include <cmp/entity.hpp>
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <cmp/physics.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shader.h>
#include <glad\glad.h>
#include <stb_image.h>
#include <cmp/entity.hpp>

namespace ECS {
Entity_t::Entity_t(glm::vec3 position)
	: ren { position }
{
	
}

Entity_t::Entity_t(const Entity_t& other) 
	: ren { other.ren.position }
{
	phy = other.phy;
	entityID = other.entityID;
}
}