#pragma once
#include <cstdint>
#include <memory>
#include <util/typealiases.hpp>

namespace ECS {

struct GameContext_t;

struct RenderSystem_t {
    explicit RenderSystem_t(uint32_t w, uint32_t h);
    ~RenderSystem_t();

    bool update(const GameContext_t& g) const;
    void drawAllEntities(const VecEntities_t& entities) const;
    // void drawEntity(const Entity_t& e) const;

    static constexpr uint32_t sprite[4*4] {
    0x0000FF00, 0x0000FF00, 0x0000FF00, 0x0000FF00,
    0x0000FF00, 0x0000FF00, 0x0000FF00, 0x0000FF00,
    0x0000FF00, 0x0000FF00, 0x0000FF00, 0x0000FF00,
    0x0000FF00, 0x0000FF00, 0x0000FF00, 0x0000FF00
    };

private :
    const uint32_t m_w { 0 }, m_h { 0 };
    std::unique_ptr<uint32_t[]> m_framebuffer { nullptr };
};
}