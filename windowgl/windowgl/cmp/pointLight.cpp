#include <cmp/pointLight.hpp>

namespace ECS {
	PointLight_t::PointLight_t(const glm::vec3& pos,
		const glm::vec3& amb, const glm::vec3& diff, const glm::vec3& spec)
		: position{ pos }, ambient{ amb }, diffuse{ diff }, specular{ spec }
	{
		
	}

	PointLight_t::PointLight_t(const glm::vec3& pos,
		const glm::vec3& amb, const glm::vec3& diff, const glm::vec3& spec,
		float constant, float linear, float quadratic)
		: position{ pos }, ambient{ amb }, diffuse{ diff }, specular{ spec }, constantDecrease{ constant },
			linearDecrease{ linear }, quadraticDecrease{ quadratic }
	{

	}
	
}