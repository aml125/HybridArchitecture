#include <sys/collision.hpp>
#include <util/gamecontext.hpp>
#include <sys/render.hpp>

namespace ECS {

bool CollisionSystem_t::update(GameContext_t& g) const {
	const VecEntities_t& vec = g.getEntities();
    for (size_t i = 0; i < vec.size(); ++i) {
		const Entity_t& e1 = vec[i];
		for (size_t j = i+1; j < vec.size(); ++j) {
			const Entity_t& e2 = vec[j];
			if (collide(e1, e2)) {
				e1.phy->position -= e1.phy->speed * RenderSystem_t::deltaTime;
				e2.phy->position -= e2.phy->speed * RenderSystem_t::deltaTime;
			}
        }
    }
    return true;
}
bool CollisionSystem_t::collide(const Entity_t& e1, const Entity_t& e2) const
{
	if (e1.phy != nullptr && e2.phy != nullptr) {
		if (linearOverlap(e1.phy->position.x, e1.collider.length.x,
			e2.phy->position.x, e2.collider.length.x)) {
			if (linearOverlap(e1.phy->position.y, e1.collider.length.y,
				e2.phy->position.y, e2.collider.length.y)) {
				if (linearOverlap(e1.phy->position.z, e1.collider.length.z,
					e2.phy->position.z, e2.collider.length.z)) {
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
}