#pragma once

#include <cstdint>
#include <cstddef>
namespace ECS {
struct Component_t {
    explicit Component_t(std::size_t eid)
        : entityID { eid }
    {}

    std::size_t ComponentID { ++nextID };
    std::size_t entityID { 0 };
private:
	inline static std::size_t nextID { 0 };
};
}