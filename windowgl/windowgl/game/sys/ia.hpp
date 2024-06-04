#pragma once

#include <game/sys/system.hpp>
#include <game\cmp\ia.hpp>
#include <game\cmp\physics.hpp>
#include <game/man/formation.hpp>
#include <game\util\timemeasure.hpp>

//Jaya parameters
#define POPULATION 256
//#define ITERATIONS 100
#define RUNS 20
#define INITIAL_SEED 100
#define HEIGHT 2

namespace GM {

#define JAYA

#ifdef JAYA
	struct OpenCLParams {
		/*OpenCLParams () 
		{

		}*/

		//Opencl stuff
		cl_mem matrixBuffer{};
		cl_mem matrixBuffer2{};
		cl_mem maxValBuffer{};
		cl_mem maxValIndexBuffer{};
		cl_mem minValBuffer{};
		cl_mem minValIndexBuffer{};
		cl_mem maxValLocalBuffer{};
		cl_mem maxValLocalIndexBuffer{};
		cl_mem minValLocalBuffer{};
		cl_mem minValLocalIndexBuffer{};
		cl_program       program;           // hold the program handler
		cl_kernel        kernel;            // hold the kernel handler
		
		//Values
		std::vector<float> matrix;
		std::vector<float> matrix2;
		/*int imin;
		int imax;
		float minVal;
		float maxVal;*/
		std::vector<float> maxVal_vec; // This was made to be able to create the buffers of this size. It makes no sense, and I should have called OpenCL functions directly instead of building a crazy library to abstract the calls
		std::vector<float> minVal_vec;
		std::vector<int> imax_vec;
		std::vector<int> imin_vec;
	};
#endif

	struct Steering {
		glm::vec3 aceleration{};
		glm::vec3 rotation{};
	};

	struct IASystem_t : System_t {
		IASystem_t(int iterations);
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
		void createBuffers(int vars);
		int totalArrived = 0;
		TimeMeasure tm;
		TimeMeasure tm2;
		bool jayaThreadLaunched = false;
		bool jayaFirstTime = true;
		int ITERATIONS = 100;

#ifdef JAYA
		OpenCLParams op{};  // Default param. The constructor sets this option
#endif

		int seed = INITIAL_SEED;
	};
}