#pragma once

#include <cmp/physics.hpp>
#include <vector>
#include <util/typealiases.hpp>

namespace ECS {

struct ComponentStorage_t {
    ComponentStorage_t() = delete;
    explicit ComponentStorage_t(std::size_t initialsize) {
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