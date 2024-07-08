#include <game/sys/ia.hpp>
#include <ecs/man/entitymanager.hpp>
#include <glm\glm\ext\vector_float3.hpp>
#include <glm\glm\geometric.hpp>
#include <game\sys\physics.hpp>
#include <game\util\log.hpp>



//TODO Refactor: All entities should't have to have a pattern assigned
//TODO make formations not count on y axis, so they walk on the terrain
namespace GM {

#ifdef JAYA
	void launchJaya(ocl_args_d_t *ocl, OpenCLParams* op, cl_ulong seed, int vars, int iterations, bool* threadLaunched);
#endif

	bool IASystem_t::threadDied() {

		return !jayaThreadLaunched;
	}

	IASystem_t::IASystem_t(int iterations)
	{
#ifdef JAYA
		ITERATIONS = iterations;
		if (ocl == NULL) {
			Log::log("ERROR OpenCLArgs (ocl) in System_t has to be set before initializing the systems");
			Log::flush();
			exit(-1);
		}
		createAndBuildProgram(ocl, op.program, "game/ocl/jaya.cl");
		createKernelFromProgram(*ocl, op.program, op.kernel, "jayaGPU");
		createKernelFromProgram(*ocl, op.program, op.kernel_min_result, "getMinResult");
#endif
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

#ifndef JAYA
		if (totalArrived < 2) {
			//Execute the pattern manager to set the new targets of the formation
			fm.updateSlots(em.getComponents<IA_t>());

			//Reset the anchor point to the center of mass of the formation
			fm.ressetAnchorsToCenterOfMass(em.getComponents <IA_t>(), em);
		}

#else
		//Check jaya Algorithm
		int vars = ((int)vecIA.size()) * 2; //TODO CHECK WHY -2 SEG FAULTS (THE FORMATION CENTER AND THE FLAG NEEDN'T BE CALCULATED)
		if (!jayaThreadLaunched) {
			if (!jayaFirstTime) {
				readJayaResults(vecIA, vars); //TODO REDO THIS FUNCTION
			}
			else {
				size_t sz = RUNS * POPULATION * (vars + 1); //Vars + 1
				Log::log(std::string("Vars: ") + std::to_string(vars));
				Log::log(std::string("Allocated size: ") + std::to_string(sz * sizeof(float) / 1024 / 1024) + "MB");
				op.matrix.resize(sz); // +1 to store min value
				op.matrix2.resize(sz);
				op.maxVal_vec.resize(RUNS);
				op.minVal_vec.resize(RUNS);
				op.imax_vec.resize(RUNS);
				op.imin_vec.resize(RUNS);
				op.result.resize(vars);
				createBuffers(vars);
			}
			jayaFirstTime = false;
			jayaThreadLaunched = true;
			/*OpenCLParams* op, std::vector<float>* matrix, std::vector<float>* matrix2, int seed, int vars, bool* threadLaunched*/
			std::thread thread(launchJaya, ocl, &op, seed++, vars, ITERATIONS, &jayaThreadLaunched);
			thread.detach();
			//launchJaya(ocl, &op, seed++, vars, ITERATIONS, &jayaThreadLaunched, tm2);
		}
#endif
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

#ifdef JAYA
	void IASystem_t::createBuffers(int vars) {
		cl_float aux = 0;
		createBuffer(*ocl, op.matrixBuffer, true, op.matrix);
		//createSimpleBuffer<cl_float>(*ocl, op.maxValBuffer, true, aux);
		//createSimpleBuffer<cl_float>(*ocl, op.minValBuffer, true, aux);
		//createSimpleBuffer<cl_float>(*ocl, op.maxValIndexBuffer, true, aux);
		//createSimpleBuffer<cl_float>(*ocl, op.minValIndexBuffer, true, aux);
		createBuffer(*ocl, op.maxValBuffer, true, op.maxVal_vec); // yeah, it does not make sense to pass the vectors to initialize empty memory...
		createBuffer(*ocl, op.minValBuffer, true, op.minVal_vec);
		createBuffer(*ocl, op.maxValIndexBuffer, true, op.imax_vec);
		createBuffer(*ocl, op.minValIndexBuffer, true, op.imin_vec);
		createBuffer(*ocl, op.matrixBuffer2, true, op.matrix2);

		createLocalParameter<cl_float>(op.kernel, 8, POPULATION * 2);
		createLocalParameter<cl_float>(op.kernel, 9, POPULATION * 2);
		createLocalParameter<int>(op.kernel, 10, POPULATION * 2);
		createLocalParameter<int>(op.kernel, 11, POPULATION * 2);

		/*copyParameter(*ocl, op.kernel, 0, op.matrixBuffer, op.matrix);
		copyParameter(*ocl, op.kernel, 12, op.matrixBuffer2, op.matrix2);*/
		setKernelArg(op.kernel, 0, sizeof(cl_mem), (void*)&op.matrixBuffer);
		setKernelArg(op.kernel, 12, sizeof(cl_mem), (void*)&op.matrixBuffer2);
	}

	void launchJaya(ocl_args_d_t *ocl, OpenCLParams* op, cl_ulong seed, int vars, int iterations, bool* threadLaunched)
	{
		TimeMeasure tm2{};
#ifdef TIMEMEASURE
		//Log::log("Starting Jaya.");
		tm2.StartCounter();
#endif
		
		cl_float aux = 0.0f;
		copyMatrixParameter<cl_float>(*ocl, op->kernel, 1, op->maxValBuffer, &aux, 1);

		copyMatrixParameter<cl_float>(*ocl, op->kernel, 2, op->minValBuffer, &aux, 1);

		copyMatrixParameter<cl_float>(*ocl, op->kernel, 3, op->maxValIndexBuffer, &aux, 1);

		copyMatrixParameter<cl_float>(*ocl, op->kernel, 4, op->minValIndexBuffer, &aux, 1);

		copySimpleParameter(op->kernel, 5, vars);
		//Log::log("Half way copied");

		copySimpleParameter(op->kernel, 6, seed);

		copySimpleParameter(op->kernel, 7, iterations);


		unsigned int globalDimensionSizes[] = { RUNS * POPULATION };
		unsigned int localDimensionSizes[] = { POPULATION };
#ifdef TIMEMEASURE
		double tm = tm2.GetCounter();
		Log::log("Jaya copying: " + std::to_string(tm));
		//Log::jayaTime(tm);
#endif
		Log::log("Executing Jaya");
#ifdef TIMEMEASURE
		//Log::log("Starting Jaya.");
		tm2.StartCounter();
#endif
		GM::executeKernel(*ocl, op->kernel, 1, globalDimensionSizes, localDimensionSizes);  //AVISO! Cambiar número de dimensiones si se cambian
		clFinish(ocl->commandQueue);  // Wait until the command has finished, to get accurate time measurements
#ifdef TIMEMEASURE
		tm = tm2.GetCounter();
		Log::log("Jaya got results: " + std::to_string(tm));
		Log::jayaTime(tm);
		tm2.StartCounter();
#endif

		//Read result
		//GM::readVectorBuffer(*ocl, op->matrixBuffer, op->matrix);
		//GM::readVectorBuffer(*ocl, op->maxValBuffer, op->maxVal_vec);
		GM::readVectorBuffer(*ocl, op->minValBuffer, op->minVal_vec);
		//GM::readVectorBuffer(*ocl, op->maxValIndexBuffer, op->imax_vec);
		GM::readVectorBuffer(*ocl, op->minValIndexBuffer, op->imin_vec);

		//
		//	Launch result kernel to find the minimum result
		//

		// Set kernel args
		GM::setKernelArg(op->kernel_min_result, 0, sizeof(cl_mem), &op->minValBuffer);
		GM::setKernelArg(op->kernel_min_result, 1, sizeof(cl_mem), &op->minValIndexBuffer);

		int s_imin = 0;
		GM::createSimpleBuffer(*ocl, op->s_iminBuffer, true, s_imin);
		GM::setKernelArg(op->kernel_min_result, 2, sizeof(cl_mem), (void*)&op->s_iminBuffer);

		int s_rmin = 0;
		GM::createSimpleBuffer<int>(*ocl, op->s_rminBuffer, true, s_rmin);
		GM::setKernelArg(op->kernel_min_result, 3, sizeof(cl_mem), (void*)&op->s_rminBuffer);

		GM::copySimpleParameter(op->kernel_min_result, 4, vars);
		GM::copySimpleParameter(op->kernel_min_result, 5, iterations);
		GM::copySimpleParameter(op->kernel_min_result, 6, POPULATION);
		GM::copySimpleParameter(op->kernel_min_result, 7, RUNS);

		// Execute kernel
		globalDimensionSizes[0] = 1;
		localDimensionSizes[0] = 1;
		GM::executeKernel(*ocl, op->kernel_min_result, 1, globalDimensionSizes, localDimensionSizes);

		// Get results
		GM::readBuffer(*ocl, op->s_iminBuffer, s_imin);
		GM::readBuffer(*ocl, op->s_rminBuffer, s_rmin);

		// Calculate index and read values
		int idx = s_rmin * POPULATION * vars + s_imin * (vars + 1);
		GM::enqueueReadBuffer(ocl->commandQueue, op->matrixBuffer, CL_TRUE, idx, vars * sizeof(float), &op->result[0], 0, NULL, NULL);

		std::cout << " imin: " << s_imin << " rmin: " << s_rmin << std::endl;

#ifdef TIMEMEASURE
		tm = tm2.GetCounter();
		Log::log("Results copied: " + std::to_string(tm));
#endif

		* threadLaunched = false;

	}

	void IASystem_t::readJayaResults(std::vector<IA_t>& vecIA, int vars)
	{
		//std::ofstream file1("results1.dat");
		for (size_t i = 0; i < vars; i += 2) {
			vecIA[i/2].target.position = { op.result[i], HEIGHT, op.result[i + 1] };
			//file1 << solution[i] << "-" << solution[i + 1] << "\n";
		}
		//file1.close();
	}
#endif

}