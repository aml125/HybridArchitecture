#include <sys/render.hpp>
#include <iostream>
#include <memory>
#include <man/entitymanager.hpp>
#include <algorithm>
#include <util/gamecontext.hpp>
#include <cmp/entity.hpp>


namespace ECS {

RenderSystem_t::RenderSystem_t(uint32_t w, uint32_t h) 
    : m_w{w}, m_h{h},
      m_framebuffer{std::make_unique<uint32_t[]>(m_w*m_h)}
{
    std::cout << "Starting render system" << std::endl;
    //ptc_open("window", w, h);
}

RenderSystem_t::~RenderSystem_t() {
    //ptc_close();
}

bool RenderSystem_t::update(const GameContext_t& g) const {
    /*auto screen = m_framebuffer.get();
    auto size = m_w*m_h;
    std::fill(screen, screen + size, 0x00FF0000);
    drawAllEntities(g.getEntities());
    
    ptc_update(screen);
    
    return !ptc_process_events();*/
	return true;
}

void RenderSystem_t::drawAllEntities(const VecEntities_t& entities) const {
    auto screen = m_framebuffer.get();

    auto getScreenXY = [&](uint32_t x, uint32_t y) { return screen + y*m_w + x; };

    auto drawEntity = [&](const Entity_t& e) {
        if (e.phy != nullptr) {
            auto screen = getScreenXY(e.phy->x, e.phy->y);
            auto sprite_it = begin(e.sprite);
            for(uint32_t y=0; y < e.h; ++y) {
                std::copy(sprite_it, sprite_it + e.w, screen);
                sprite_it += e.w;
                screen += m_w;
            }
        }
    };
    std::for_each(begin(entities), end(entities), drawEntity);

     uint32_t *screenPtr = screen;
     const uint32_t *spritePtr = sprite;

     //Draw sprite
     for (uint32_t i = 0; i < 4; i++) {
         for (uint32_t j = 0; j < 4; j++) {
             *screenPtr = *spritePtr;
             screenPtr++;
             spritePtr++;
         }
         screenPtr += 640 - 4;
     }

     //ptc_update(screen);
}

// void RenderSystem_t::drawEntity(const Entity_t& e) const {
//     auto screen = m_framebuffer.get();
//     screen += e.y*m_w + e.x;
//     auto sprite_it = begin(e.sprite);
//     for(uint32_t y=0; y < e.h; ++y) {
//         std::copy(sprite_it, 
//             sprite_it + e.w, screen);
//         sprite_it += e.w;
//         screen += m_w;
//     }
// }

}