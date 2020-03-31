#include <sys/collision.hpp>
#include <util/gamecontext.hpp>
#include <sys/render.hpp>

namespace ECS {

bool CollisionSystem_t::update(GameContext_t& g) const {
	const VecEntities_t& vec = g.getEntities();
    for (size_t i = 0; i < vec.size(); ++i) {
		const Entity_t& e1 = vec[i];
		//TODO QUITAR DUPLICIDAD
		for (size_t j = i+1; j < vec.size(); ++j) {
			const Entity_t& e2 = vec[j];
			if (collide(e1, e2)) {
				e1.phy->position -= e1.phy->speed * RenderSystem_t::deltaTime;
				e2.phy->position -= e2.phy->speed * RenderSystem_t::deltaTime;
				e1.phy->speed = { 0, 0, 0 };
				e2.phy->speed = { 0, 0, 0 };
			}
        }
    }
    return true;
}
bool CollisionSystem_t::collide(const Entity_t& e1, const Entity_t& e2) const
{
	//Add the offset to the position
	glm::vec3 pos1 = calculatePosition(e1.phy->position, e1.collider.length, e1.collider.offset);
	glm::vec3 pos2 = calculatePosition(e2.phy->position, e2.collider.length, e2.collider.offset);

	//TODO QUITAR DUPLICIDAD
	if (e1.phy != nullptr && e2.phy != nullptr) {
		if (linearOverlap(pos1.x, e1.collider.length.x,
			pos2.x, e2.collider.length.x)) {
			if (linearOverlap(pos1.y, e1.collider.length.y,
				pos2.y, e2.collider.length.y)) {
				if (linearOverlap(pos1.z, e1.collider.length.z,
					pos2.z, e2.collider.length.z)) {
					return true;
				}
			}
		}
	}
	return false;
}
bool CollisionSystem_t::linearOverlap(float x1, float w1, float x2, float w2) const
{
	return (x1 + w1 >= x2&& x1 + w1 <= x2 + w2) || (x2 + w2 >= x1&& x2 + w2 <= x1 + w1);
}

glm::vec3 CollisionSystem_t::calculatePosition(const glm::vec3& position, const glm::vec3& length, const glm::vec3& offset) const {
	glm::vec3 result = { position.x - (length.x / 2) + offset.x,
		position.y - (length.y / 2) + offset.y,  // Length / 2 porque asi se define el origen en el centro de la caja,
		position.z - (length.z / 2) + offset.z	 // por lo que length es el tamaño del lado de la caja
	};
	return result;
}
}