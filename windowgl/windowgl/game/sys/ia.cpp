#include <game/sys/ia.hpp>
#include <ecs/man/entitymanager.hpp>
#include <glm\glm\ext\vector_float3.hpp>
#include <glm\glm\geometric.hpp>
#include <game\sys\physics.hpp>
#include <game\util\log.hpp>

//TODO Refactor: All entities should't have to have a pattern assigned
//TODO make formations not count on y axis, so they walk on the terrain
namespace GM {

	bool IASystem_t::threadDied() {

		return !jayaThreadLaunched;
	}

	void launchJaya(OpenCLParams* op, cl_ulong seed, int vars, bool* threadLaunched);

	IASystem_t::IASystem_t() {
		createAndBuildProgram(&op.ocl, op.program, "game/ocl/jaya.cl");
		createKernelFromProgram(op.ocl, op.program, op.kernel, "jayaGPU");
	}

	void IASystem_t::update(ECS::EntityManager_t& em) {
#ifdef TIMEMEASURE
		tm.StartCounter();
#endif
		//Execute the steering behaviours to get to the targets
		auto& vecIA = em.getComponents<IA_t>();
		for (IA_t& ia : vecIA) {
			auto* phy = em.getEntity(ia.entityID).getComponent<PhysicsComponent_t>();
			if (phy == nullptr) {
				Log::log("IASystem_t::update() ERROR cant have ia without Physics System");
				return;
			}
			stateMachineUpdate(em, ia, *phy);
			
		}

		//if (totalArrived < 2) {
		//	//Execute the pattern manager to set the new targets of the formation
		//	fm.updateSlots(em.getComponents<IA_t>());

		//	//Reset the anchor point to the center of mass of the formation
		//	fm.ressetAnchorsToCenterOfMass(em.getComponents <IA_t>(), em);
		//}

		//Check jaya Algorithm
		if (!jayaThreadLaunched) {
			if (!jayaFirstTime) {
				#ifdef TIMEMEASURE
								Log::log("Jaya got results: " + std::to_string(tm2.GetCounter()));
								tm2.StartCounter();
				#endif
				readJayaResults(vecIA, vecIA.size());
			}
			jayaFirstTime = false;
			jayaThreadLaunched = true;
			/*OpenCLParams* op, std::vector<float>* matrix, std::vector<float>* matrix2, int seed, int vars, bool* threadLaunched*/
			std::thread thread(launchJaya, &op, seed++, (int)vecIA.size(), &jayaThreadLaunched);
			thread.detach();
			//launchJaya(ocl, &op, &matrix, &matrix2, seed, (int)vecIA.size(), &jayaThreadLaunched);
		}
#ifdef TIMEMEASURE
		Log::log("IA: " + std::to_string(tm.GetCounter()));
#endif
	}

	Steering IASystem_t::arrive(const IA_t& ia, const PhysicsComponent_t& phy, bool& arrived)
	{
		Steering steering; //Return data
		arrived = false;
		
		glm::vec3 direction = ia.target.position - phy.position;
		float distance = glm::length(direction);
		//std::cout << "Distance: " << distance << std::endl;

		//If we are in the target radius, we arived
		if (distance <= ia.targetRadius) {
			steering.aceleration = -phy.speed;
			//std::cout << "arrived" << std::endl;
			arrived = true;
			return steering;
		}

		//If outside of the slow radius, go full speed
		float targetSpeed = 0;
		if (distance > ia.slowRadius) {
			targetSpeed = ia.maxSpeed;
			//std::cout << "outside slow radius\n";
		}
		else {
			//Else calculate speed
			targetSpeed = ia.maxSpeed * distance / ia.slowRadius;
			//std::cout << "Inside slow radius\n";
		}

		//Target velocity combines speed and direction
		glm::vec3 targetVelocity = glm::normalize(direction);
		targetVelocity *= targetSpeed;

		//Calculate aceleration to get to the target velocity. Takes into account acceleration time
		steering.aceleration = (targetVelocity - phy.speed) / ia.timeToTarget;
		
		//Check if acceleration is too fast
		if (glm::length(steering.aceleration) > ia.maxAcceleration) {
			steering.aceleration = glm::normalize(steering.aceleration);
			steering.aceleration *= ia.maxAcceleration;
			//std::cout << "Too fast\n";
		}
		setOrientation(steering.rotation, phy.speed); //TODO Don set the rotation directly, but with a target and acceleration
		/*std::cout << "Position: x:" << phy.position.x << " y:" << phy.position.y << " z:" << phy.position.z << std::endl;
		std::cout << "Target: x:" << ia.target.position.x << " y:" << ia.target.position.y << " z:" << ia.target.position.z << std::endl;
		<< "Acell: x:" << steering.aceleration.x << " y:" << steering.aceleration.y << " z:" << steering.aceleration.z << std::endl << std::endl;*/
		return steering;
	}

	void IASystem_t::setOrientation(glm::vec3& orientation, const glm::vec3& velocity)
	{
		if (glm::length(velocity) > 0) {
			orientation.y = std::atan2(velocity.x, velocity.z);
		}
	}

	void IASystem_t::stateMachineUpdate(ECS::EntityManager_t& em, IA_t& ia, PhysicsComponent_t& phy)
	{
		bool arrived = false;
		Steering* steering;
		//Hard coded state machine
		switch (ia.state) {
		case ARRIVE:
			steering = &arrive(ia, phy, arrived);
			phy.aceleration = steering->aceleration;
			phy.rotation = steering->rotation; //TODO Don set the rotation directly, but with a target and acceleration

			if (arrived) {
				ia.state = WAIT;
			}
			break;
		case ANCHOR_WAIT:

			break;
		case ANCHOR_ARRIVE:
		{
			bool setFormation = setFormationTarget(em, ia, findOtherFormation(em, ia));
			bool arrived = false;
			steering = &arrive(ia, phy, arrived);
			phy.aceleration = steering->aceleration;
			if (setFormation && arrived) {
				Log::log("Anchor arrived");
				ia.state = ANCHOR_WAIT;
				totalArrived++;
				//Cancel aceleration and speed of the formation anchor
				phy.aceleration = { 0, 0, 0 };
				phy.speed = { 0, 0, 0 };
			}
			break;
		}
		case WAIT:
			if (ia.isAnchorPoint) {
				ia.state = ANCHOR_ARRIVE;
			}
			//else if (totalArrived == 2) {
			//	////find closest enemy
			//	//ia.enemyID = findClosestEnemy(em, phy.position, ia.patternNumber);
			//	//ia.state = GO;
			//}
			else if (targetHasMoved(ia, phy)) {
				ia.state = ARRIVE;
			}
			break;
		case GO:
			//Chase the enemy
			setTargetOnRangeOfAttack(em, ia);
			steering = &arrive(ia, phy, arrived);
			phy.aceleration = steering->aceleration;
			phy.rotation = steering->rotation;
			if (arrived) {
				ia.state = ATTACK;
			}
			break;
		case ATTACK:
			ia.state = BACK;
			break;
		case BACK:
			fm.updateSlot(ia);
			steering = &arrive(ia, phy, arrived);
			phy.aceleration = steering->aceleration;
			phy.rotation = steering->rotation;
			if (arrived) {
				ia.state = GO;
			}
			break;
		}
	}
	bool IASystem_t::targetHasMoved(IA_t& ia, PhysicsComponent_t& phy)
	{
		glm::vec3 direction = ia.target.position - phy.position;
		float distance = glm::length(direction);
		return distance > ia.targetRadius;
	}

	//Sets the target for the formation and returns true if already arrived
	bool IASystem_t::setFormationTarget(ECS::EntityManager_t& em, IA_t& ia, const PhysicsComponent_t& targetPhy)
	{
		auto* myPhy = em.getEntity(ia.entityID).getComponent<PhysicsComponent_t>();
		glm::vec3 direction = targetPhy.position - myPhy->position;
		ia.target.position.x = targetPhy.position.x;
		if (direction.z > 0) {
			ia.target.position.z = targetPhy.position.z - 3; //- or + x to let x space between formations
		}
		else {
			ia.target.position.z = targetPhy.position.z + 3;
		}

		if (myPhy->position.x == targetPhy.position.x) {
			return true;
		}
		return false;
	}

	//TODO: Optimize to dont search through all ia list every time
	PhysicsComponent_t& IASystem_t::findOtherFormation(ECS::EntityManager_t& em, IA_t& myFormation)
	{
		for (IA_t& ia : em.getComponents<IA_t>()) {
			if (ia.isAnchorPoint && ia.entityID != myFormation.entityID) {
				return *em.getEntity(ia.entityID).getComponent<PhysicsComponent_t>();
			}
		}
		Log::log("EROR: PhysicsComponent_t& IASystem_t::findOtherFormation(...) No other formation found");
		exit(-1);
		return *em.getEntity(myFormation.entityID).getComponent<PhysicsComponent_t>(); //JUST TO AVOID NON RETURN ERROR
	}

	unsigned int IASystem_t::findClosestEnemy(const ECS::EntityManager_t& em, const glm::vec3& pos, unsigned int formation) {
		bool first = true;
		float min = 0;
		unsigned int targetID = 999;
		for (auto& ia : em.getComponents<IA_t>()) {
			if (ia.patternNumber != formation && ia.patternNumber < 2) {
				auto* phy = em.getEntity(ia.entityID).getComponent<PhysicsComponent_t>();
				float distance = std::abs(glm::length(phy->position - pos));
				if (first || distance < min) {
					min = distance;
					targetID = phy->entityID;
					first = false;
				}
			}
		}
		Log::log("TARGET: " + std::to_string(targetID));
		return targetID;
	}

	void IASystem_t::setTargetOnRangeOfAttack(const ECS::EntityManager_t& em, IA_t& ia) {
		auto* enemyPhy = em.getEntity(ia.enemyID).getComponent<PhysicsComponent_t>();
		auto* myPhy = em.getEntity(ia.entityID).getComponent<PhysicsComponent_t>();
		if (enemyPhy == nullptr || myPhy == nullptr) {
			Log::log("ERROR: void setTargetOnRangeOfAttack(const ECS::EntityManager_t& em, IA_t& ia) IA of enemy has no physic component");
			exit(-1);
		}
		glm::vec3 distance = enemyPhy->position - myPhy->position;
		if (distance.z > 0) {
			ia.target.position.z = enemyPhy->position.z - 0.5;
		}
		else {
			ia.target.position.z = enemyPhy->position.z + 0.5;
		}
		ia.target.position.x = enemyPhy->position.x;
	}

	//const int population = 512;
	//const cl_int vars = 256;
	//const cl_ulong seed = 100;
	////const int iterations = 100;
	//const int runs = 10000;

	////Opencl stuff
	//cl_mem matrixBuffer{};
	//cl_mem matrixBuffer2{};
	//cl_mem varsBuffer{};
	//cl_mem seedBuffer{};
	//cl_mem maxValBuffer{};
	//cl_mem maxValIndexBuffer{};
	//cl_mem minValBuffer{};
	//cl_mem minValIndexBuffer{};
	//cl_mem maxValLocalBuffer{};
	//cl_mem maxValLocalIndexBuffer{};
	//cl_mem minValLocalBuffer{};
	//cl_mem minValLocalIndexBuffer{};
	//cl_mem minMaxBuffer{};

	//std::vector<unsigned _int64> vecIndex;
	//cl_program       program;           // hold the program handler
	//cl_kernel        kernel;            // hold the kernel handler

	void launchJaya(OpenCLParams* op, cl_ulong seed, int vars, bool* threadLaunched)
	{
		op->matrix.resize(POPULATION * (vars + 1)); // +1 to store min value

		createBuffer(op->ocl, op->matrixBuffer, true, op->matrix);
		copyParameter(op->ocl, op->kernel, 0, op->matrixBuffer, op->matrix);

		float aux = 0;
		createSimpleBuffer(op->ocl, op->maxValBuffer, true, aux);
		copyMatrixParameter<float>(op->ocl, op->kernel, 1, op->maxValBuffer, &aux, 1);

		createSimpleBuffer(op->ocl, op->minValBuffer, true, aux);
		copyMatrixParameter<float>(op->ocl, op->kernel, 2, op->minValBuffer, &aux, 1);

		createSimpleBuffer(op->ocl, op->maxValIndexBuffer, true, aux);
		copyMatrixParameter<float>(op->ocl, op->kernel, 3, op->maxValIndexBuffer, &aux, 1);

		createSimpleBuffer(op->ocl, op->minValIndexBuffer, true, aux);
		copyMatrixParameter<float>(op->ocl, op->kernel, 4, op->minValIndexBuffer, &aux, 1);

		copySimpleParameter(op->ocl, op->kernel, 5, vars);

		copySimpleParameter(op->ocl, op->kernel, 6, seed);

		int iterations = ITERATIONS;
		copySimpleParameter(op->ocl, op->kernel, 7, iterations);

		//copySimpleParameter(op->ocl, kernel, 7, runs);


		createLocalParameter<float>(op->kernel, 8, POPULATION * 2);
		createLocalParameter<float>(op->kernel, 9, POPULATION * 2);
		createLocalParameter<int>(op->kernel, 10, POPULATION * 2);
		createLocalParameter<int>(op->kernel, 11, POPULATION * 2);

		op->matrix2.resize(POPULATION * vars);

		createBuffer(op->ocl, op->matrixBuffer2, true, op->matrix2);
		copyParameter(op->ocl, op->kernel, 12, op->matrixBuffer2, op->matrix2);

		unsigned int globalDimensionSizes[] = { POPULATION };
		unsigned int localDimensionSizes[] = { POPULATION };
		GM::executeKernel(op->ocl, op->kernel, 1, globalDimensionSizes, localDimensionSizes);  //AVISO! Cambiar número de dimensiones si se cambian

		//Read result
		GM::readVectorBuffer(op->ocl, op->matrixBuffer, op->matrix);
		GM::readBuffer(op->ocl, op->maxValBuffer, op->maxVal);
		GM::readBuffer(op->ocl, op->minValBuffer, op->minVal);
		GM::readBuffer(op->ocl, op->maxValIndexBuffer, op->imax);
		GM::readBuffer(op->ocl, op->minValIndexBuffer, op->imin);

		*threadLaunched = false;


		//GM::ocl_args_d_t ocl{};
		//createAndBuildProgram(&ocl, program, "game/ocl/jaya.cl");
		//createKernelFromProgram(ocl, program, kernel, "jayaGPU");

		////Reserve memory for the matrix
		///*double* matrix = new double [population * vars];*/
		//std::vector<float> matrix;
		//matrix.resize(population * (vars + 1)); // +1 to store min value

		//GM::createBuffer(ocl, matrixBuffer, true, matrix);
		//GM::copyParameter(ocl, kernel, 0, matrixBuffer, matrix);

		//float aux = 0;
		//GM::createSimpleBuffer(ocl, maxValBuffer, true, aux);
		//GM::copyMatrixParameter<float>(ocl, kernel, 1, maxValBuffer, &aux, 1);

		//GM::createSimpleBuffer(ocl, minValBuffer, true, aux);
		//GM::copyMatrixParameter<float>(ocl, kernel, 2, minValBuffer, &aux, 1);

		//GM::createSimpleBuffer(ocl, maxValIndexBuffer, true, aux);
		//GM::copyMatrixParameter<float>(ocl, kernel, 3, maxValIndexBuffer, &aux, 1);

		//GM::createSimpleBuffer(ocl, minValIndexBuffer, true, aux);
		//GM::copyMatrixParameter<float>(ocl, kernel, 4, minValIndexBuffer, &aux, 1);

		//GM::copySimpleParameter(ocl, kernel, 5, vars);

		//GM::copySimpleParameter(ocl, kernel, 6, seed);

		////GM::copySimpleParameter(ocl, kernel, 7, iterations);

		//GM::copySimpleParameter(ocl, kernel, 7, runs);


		//GM::createLocalParameter<float>(kernel, 8, population * 2);
		//GM::createLocalParameter<float>(kernel, 9, population * 2);
		//GM::createLocalParameter<int>(kernel, 10, population * 2);
		//GM::createLocalParameter<int>(kernel, 11, population * 2);

		//std::vector<float> matrix2;
		//matrix2.resize(population * vars);

		//GM::createBuffer(ocl, matrixBuffer2, true, matrix2);
		//GM::copyParameter(ocl, kernel, 12, matrixBuffer2, matrix2);

		////std::vector<float> minMaxMatrix;
		////minMaxMatrix.resize(population * 2); //every individual stores 2 values, min and max
		////GM::createBuffer(ocl, minMaxBuffer, true, minMaxMatrix);
		////GM::copyParameter(ocl, kernel, 14, minMaxBuffer, minMaxMatrix);

		//std::size_t result;
		//cl_int maxLocal = clGetKernelWorkGroupInfo(kernel, NULL, CL_KERNEL_WORK_GROUP_SIZE, sizeof(std::size_t), &result, NULL);
		//if (maxLocal != 0) {
		//	std::cout << "Ha ocurrido algun error" << std::endl;
		//}

		//std::cout << "Max work group size: " << result << std::endl;

		///*unsigned int globalDimensionSizes[] = { runs, population };
		//unsigned int localDimensionSizes[] = { 1, population };*/
		//unsigned int globalDimensionSizes[] = { population };
		//unsigned int localDimensionSizes[] = { population };
		//GM::executeKernel(ocl, kernel, 1, globalDimensionSizes, localDimensionSizes);  //AVISO! Cambiar número de dimensiones si se cambian

		//float maxVal, minVal;
		//int imax, imin;
		//GM::readVectorBuffer(ocl, matrixBuffer, matrix);
		//GM::readBuffer(ocl, maxValBuffer, maxVal);
		//GM::readBuffer(ocl, minValBuffer, minVal);
		//GM::readBuffer(ocl, maxValIndexBuffer, imax);
		//GM::readBuffer(ocl, minValIndexBuffer, imin);

	}

	void IASystem_t::readJayaResults(std::vector<IA_t>& vecIA, int vars)
	{
		float* solution = &op.matrix[op.imin * (vars + 1)];
		std::ofstream file1("results1.dat");
		for (size_t i = 0; i < vars / 2; i += 2) {
			vecIA[i].target.position = { solution[i], HEIGHT, solution[i + 1] };
		}
	}


}