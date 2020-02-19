#pragma once

#include <cmp/physics.hpp>
#include <vector>
#include <util/typealiases.hpp>
#include <iostream>

namespace ECS {

struct ComponentStorage_t {
    ComponentStorage_t() = delete;
    explicit ComponentStorage_t(std::size_t initialsize) {
		std::vector<int> myvec;
		myvec.reserve(3);
        m_physicsComponents.reserve(initialsize);
    }

    ComponentStorage_t(ComponentStorage_t&) = delete;
    ComponentStorage_t(ComponentStorage_t&&) = delete;
    ComponentStorage_t& operator=(ComponentStorage_t&) = delete;
    ComponentStorage_t& operator=(ComponentStorage_t&&) = delete;

    const VecPhysics_t& getPhysicsComponents() const { return getPhysicsComponents(); };
    VecPhysics_t& getPhysicsComponents() { return m_physicsComponents; };
    PhysicsComponent_t& createPhysycsComponent();
private:
    VecPhysics_t m_physicsComponents {};
};
}