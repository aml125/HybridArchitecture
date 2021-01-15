#pragma once

#include <cstdint>
#include <vector>
#include <ecs/man/componentstorage.hpp>
#include <ecs/util/typealiases.hpp>
#include <ecs\cmp\entity.hpp>

namespace ECS {

struct EntityManager_t {

    static constexpr std::size_t kNUMINITALLENTITIES { 50000 };

    explicit EntityManager_t();

    Entity_t& createEntity();
    const VecEntities_t& getEntities() const { return m_Entity; };
    VecEntities_t& getEntities() { return m_Entity; };
    const Entity_t& getEntity(std::size_t eid) const {
        return m_Entity[eid];
    }
    Entity_t& getEntity(std::size_t eid) {
        return m_Entity[eid];
    }
    template <typename CMP_t>
    std::vector<CMP_t>& getComponents() {return m_components.getComponents<CMP_t>(); };
    template <typename CMP_t>
    const std::vector<CMP_t>& getComponents() const { return m_components.getComponents<CMP_t>(); };
    template <typename CMP_t>
    CMP_t& createComponent(std::size_t eid) {
        return m_components.createComponent<CMP_t>(eid);
    }
private:
    VecEntities_t m_Entity {};
    ComponentStorage_t m_components { kNUMINITALLENTITIES };
};
}
//siharaib@mindfase.tk