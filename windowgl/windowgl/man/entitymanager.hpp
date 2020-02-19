#pragma once

#include <cstdint>
#include <vector>
#include <man/componentstorage.hpp>
#include <util/typealiases.hpp>
#include <util/gamecontext.hpp>

namespace ECS {

struct EntityManager_t;

struct EntityManager_t : GameContext_t {

    static constexpr std::size_t kNUMINITALLENTITIES { 1000 };

    explicit EntityManager_t();

    void createEntity(uint32_t x, uint32_t y, std::string filename);
    const VecEntities_t& getEntities() const override { return m_Entity; };
    VecEntities_t& getEntities() override { return m_Entity; };
    virtual VecPhysics_t& getPhysicsComponents() override {return m_components.getPhysicsComponents(); };
    virtual const VecPhysics_t& getPhysicsComponents() const { return m_components.getPhysicsComponents(); };
private:
    VecEntities_t m_Entity {};
    ComponentStorage_t m_components { kNUMINITALLENTITIES };
};
}