#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <picoPNG/src/picopng.hpp>
#include <fstream>
#include <cstring>
#include <cmp/physics.hpp>

namespace ECS {

struct Entity_t {
    explicit Entity_t(uint32_t _w, uint32_t _h, uint32_t _vx, uint32_t _vy)
      : w(_w), h(_h)
    {
        const uint32_t size = w*h;
        sprite.reserve(size);
        sprite.resize(size);
    }
    
    explicit Entity_t(std::string filename, uint32_t _vx, uint32_t _vy) {
        std::vector<unsigned char> pixels {};
        unsigned long dw {0}, dh {0};
        std::ifstream file{filename, std::ios::binary}; //No preocuparse del close
        if (!file.is_open()) {
            throw new std::string("EXCEPTION: File could not be open.");
        }
        std::vector<unsigned char> filevec(
            // iterador principio file
            std::istreambuf_iterator<char>(file),
            // iterador final file
            std::istreambuf_iterator<char>{}
        );
        
        decodePNG(pixels, dw, dh, filevec.data(), filevec.size());
        sprite.resize(pixels.size());
        std::memcpy(sprite.data(), pixels.data(), pixels.size());
        // sprite = std::vector<uint32_t>(
        //     pixels.begin(), pixels.end()
        // );
        w = dw;
        h = dh;
    }
    PhysicsComponent_t* phy {};
    uint32_t w { 0 }, h { 0 };
    std::vector<uint32_t> sprite {};
    std::size_t entityID { ++nextID };

    inline static std::size_t nextID { 0 };
};
}