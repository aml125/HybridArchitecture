#pragma once

#include <vector>
#include <ecs/util/typealiases.hpp>
#include <iostream>
#include <unordered_map>

namespace ECS {

struct ComponentVectorBase_t {
    virtual ~ComponentVectorBase_t() = default;
};

template <typename CMP_t>
struct ComponentVector_t : ComponentVectorBase_t {
    std::vector<CMP_t> components;
};

struct ComponentStorage_t {
    ComponentStorage_t() = delete;
    explicit ComponentStorage_t(std::size_t initialsize) {
        vectorSize = initialsize;
    }

    ComponentStorage_t(ComponentStorage_t&) = delete;
    ComponentStorage_t(ComponentStorage_t&&) = delete;
    ComponentStorage_t& operator=(ComponentStorage_t&) = delete;
    ComponentStorage_t& operator=(ComponentStorage_t&&) = delete;

    inline static unsigned int idCounter = 0;

    template <typename CMP_t>
    std::vector<CMP_t>& createComponentVector() {
        auto v = std::make_unique<ComponentVector_t<CMP_t>>();
        int id = ComponentStorage_t::getComponentId<CMP_t>();
        v->components.reserve(vectorSize);

        auto* auxPtr = v.get();
        componentVector[id] = std::move(v);
        return auxPtr->components;
    }

    template <typename CMP_t>
    std::vector<CMP_t>& getComponents() {
        std::vector<CMP_t>* auxPtr = nullptr;
        auto it = componentVector.find(getComponentId<CMP_t>());
        if (it != componentVector.end()) {
            auxPtr = &dynamic_cast<ComponentVector_t<CMP_t>*>(it->second.get())->components;
        }
        else {
            auxPtr = &createComponentVector<CMP_t>();
        }
        return *auxPtr;
    }

    template <typename CMP_t>
    const std::vector<CMP_t>& getComponents() const {
        std::vector<CMP_t>* auxPtr = nullptr;
        auto it = componentVector.find(ComponentStorage_t::getComponentId<CMP_t>());
        if (it != componentVector.end()) {
            auxPtr = &dynamic_cast<ComponentVector_t<CMP_t>*>(it->second.get())->components;
        }
        else {
            exit(-4); //BIG ERROR
        }
        return *auxPtr;
    }

    template <typename ComponentType>
    static unsigned int getComponentId() {
        static unsigned int id = ++idCounter;
        return id;
    }

    template <typename CMP_t>
    CMP_t& createComponent(std::size_t eid) {
        auto& vec = getComponents<CMP_t>();
        auto& cmp = vec.emplace_back(eid);
        return cmp;
    }
private:
    //std::vector<PhysicsComponent_t> m_physicsComponents {};
    std::unordered_map<int, std::unique_ptr<ComponentVectorBase_t>> componentVector;
    std::size_t vectorSize;
};
}