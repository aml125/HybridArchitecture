#pragma once

#include <game/sys/system.hpp>
#include <game\cmp\ia.hpp>
#include <game\cmp\physics.hpp>
#include <game/man/formation.hpp>
#include <game\util\timemeasure.hpp>

//Jaya parameters
#define POPULATION 512
#define ITERATIONS 50
#define RUNS 50
#define INITIAL_SEED 100
#define HEIGHT 2

namespace GM {
	struct OpenCLParams {
		//Opencl stuff
		cl_mem matrixBuffer{};
		cl_mem matrixBuffer2{};
		cl_mem varsBuffer{};
		cl_mem seedBuffer{};
		cl_mem maxValBuffer{};
		cl_mem maxValIndexBuffer{};
		cl_mem minValBuffer{};
		cl_mem minValIndexBuffer{};
		cl_mem maxValLocalBuffer{};
		cl_mem maxValLocalIndexBuffer{};
		cl_mem minValLocalBuffer{};
		cl_mem minValLocalIndexBuffer{};
		cl_mem minMaxBuffer{};
		GM::ocl_args_d_t ocl{std::string("NVIDIA"), true};
		cl_program       program;           // hold the program handler
		cl_kernel        kernel;            // hold the kernel handler
		
		//Values
		std::vector<float> matrix;
		std::vector<float> matrix2;
		int imin;
		int imax;
		float minVal;
		float maxVal;
	};

	struct Steering {
		glm::vec3 aceleration{};
		glm::vec3 rotation{};
	};

	struct IASystem_t : System_t {
		IASystem_t();
		void update(ECS::EntityManager_t& em) override;
		bool threadDied();
		FormationManager fm{};

	private:
		Steering arrive(const IA_t& ia, const PhysicsComponent_t& phy, bool& arrived);
		void setOrientation(glm::vec3& orientation, const glm::vec3& velocity);
		void stateMachineUpdate(ECS::EntityManager_t& em, IA_t& ia, PhysicsComponent_t& phy);
		bool targetHasMoved(IA_t& ia, PhysicsComponent_t& phy);
		bool setFormationTarget(ECS::EntityManager_t& em, IA_t& ia, const PhysicsComponent_t& targetPhy);
		PhysicsComponent_t& findOtherFormation(ECS::EntityManager_t& em, IA_t& myFormation);
		unsigned int findClosestEnemy(const ECS::EntityManager_t& em, const glm::vec3& pos, unsigned int formation);
		void setTargetOnRangeOfAttack(const ECS::EntityManager_t& em, IA_t& ia);
		void readJayaResults(std::vector<IA_t>& vecIA, int vars);
		int totalArrived = 0;
		TimeMeasure tm;
		TimeMeasure tm2;
		bool jayaThreadLaunched = false;
		bool jayaFirstTime = true;

		OpenCLParams op{};

		int seed = INITIAL_SEED;
	};
}