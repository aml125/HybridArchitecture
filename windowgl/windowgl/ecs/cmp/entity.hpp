#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <unordered_map>
#include <ecs\cmp\component.hpp>

namespace ECS {

struct Entity_t {
	explicit Entity_t() = default;

	template <typename CMP_t>
	CMP_t& addComponent(CMP_t& cmp) {
		components[ComponentStorage_t::getComponentId<CMP_t>()] = &cmp;
		return cmp;
	};

	template <typename CMP_t>
	CMP_t* getComponent() {
		unsigned int id = ComponentStorage_t::getComponentId<CMP_t>();
		auto it = components.find(id);

		if (it != components.end()) {
			return static_cast<CMP_t*>(it->second);
		}
		return nullptr;
	}

	template <typename CMP_t>
	const CMP_t* getComponent() const {
		auto* cmp = const_cast<Entity_t*>(this)->getComponent<CMP_t>();
		return const_cast<const CMP_t*>(cmp);
	}

	/*const Model_t& getModel() const { return model; };*/
    std::size_t entityID { nextID++ };

private:
	std::unordered_map<int, Component_t*> components{};
	inline static std::size_t nextID{ 0 };
};
}