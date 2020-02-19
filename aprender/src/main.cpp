extern "C" {
    #include <TinyPTC/src/tinyptc.h>
}

#include <cstdint>
#include <iostream>
#include <sys/render.hpp>
#include <man/entitymanager.hpp>
#include <sys/physics.hpp>
#include <sys/collision.hpp>

constexpr uint32_t kSCRWIDTH { 640 };
constexpr uint32_t kSCRHEIGHT { 360 };

// uint32_t screen[640*360];

// struct Screen_t {
//     Screen_t(uint32_t w, uint32_t h) 
//     : screen(new uint32_t[w*h]) {}

//     ~Screen_t() {

//     }

//     uint32_t screen[nullptr];
// }

void game() {
    // ptc_open("window", kSCRWIDTH, kSCRHEIGHT);

    

    

    ptc_close();
}

int main() {
    try {
        ECS::EntityManager_t EntityMan;
        EntityMan.createEntity(20, 40, "assets/cactus.png");
        ECS::PhysicsSystem_t Physics;
        ECS::CollisionSystem_t Collision;
        const ECS::RenderSystem_t Render(kSCRWIDTH, kSCRHEIGHT);
        while (Render.update(EntityMan)) {
            Physics.update(EntityMan);
            Collision.update(EntityMan);
        }
    } catch(std::string ex) {
        std::cout << "Capturada : " + ex;
    } catch(...) {
        std::cout << "Capturada : Excepción desconocida.";
    }

    return 0;
}