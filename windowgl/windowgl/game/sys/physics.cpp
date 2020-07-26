#include <game/sys/physics.hpp>
#include <game/sys/render.hpp>
#include <game\util\log.hpp>


namespace GM {
    PhysicsSystem_t::PhysicsSystem_t()
    {
        createAndBuildProgram(&ocl, program, "game/ocl/physics_ocl.cl");
        createKernelFromProgram(ocl, program, kernel, "update");
        deltaTimeBuffer = createFloatParam(ocl, RenderSystem_t::deltaTime);
    }


    void PhysicsSystem_t::update(ECS::EntityManager_t& g) {
#ifdef TIMEMEASURE
    tm.StartCounter();
#endif
    if (firstTime) {
        firstTime = false;
        return;
    }
    //GPU Implementation
    std::vector<PhysicsComponent_t>& vecPhy = g.getComponents<PhysicsComponent_t>();
    if (vecPhy.size() != lastPhysicsVectorSize) {
        createBuffer(ocl, phyBuffer, true, vecPhy);
        lastPhysicsVectorSize = vecPhy.size();
    }
    copyParameters(ocl, kernel, 0, phyBuffer, vecPhy);
    copyFloatParam(ocl, kernel, 1, deltaTimeBuffer, RenderSystem_t::deltaTime);
    unsigned int dimensionSizes[] = { vecPhy.size() };
    executeKernel(ocl, kernel, 1, dimensionSizes);
    readBuffer(ocl, phyBuffer, vecPhy);

    //CPU Implementation
   /* for (auto& phy : g.getComponents<PhysicsComponent_t>()) {
        if (phy.gravity) {
            aplyGravity(phy);
        }
        moveObject(phy);
    }*/
#ifdef TIMEMEASURE
    Log::log("Physics: " + std::to_string(tm.GetCounter()));
#endif
}

void PhysicsSystem_t::aplyGravity(PhysicsComponent_t& cmp) const {
    cmp.speed += cmp.gravityAcell * RenderSystem_t::deltaTime;
}
void PhysicsSystem_t::moveObject(PhysicsComponent_t& phy) const
{
    phy.speed += phy.aceleration * RenderSystem_t::deltaTime;
    phy.position += phy.speed * RenderSystem_t::deltaTime;
    phy.rotationSpeed += phy.rotationAceleration * RenderSystem_t::deltaTime;
    phy.rotation += phy.rotationSpeed * RenderSystem_t::deltaTime;
}
}