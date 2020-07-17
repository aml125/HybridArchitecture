// TODO: Add OpenCL kernel code here.

#include <game/ocl/ocl_structs.cl>

void aplyGravity(__global struct PhysicsComponent_ocl_t* cmp, float deltaTime) {
	struct mivec3_t aux;
	multiplyVectorByScalarGL(&cmp->gravityAcell, deltaTime, &aux);
	addVectorLG(&aux, &cmp->speed);
}

void moveObject(__global struct PhysicsComponent_ocl_t* phy, float deltaTime) {
	struct mivec3_t aux;
	multiplyVectorByScalarGL(&phy->aceleration, deltaTime, &aux);
	addVectorLG(&aux, &phy->speed);
	multiplyVectorByScalarGL(&phy->speed, deltaTime, &aux);
	addVectorLG(&aux, &phy->position);
	multiplyVectorByScalarGL(&phy->rotationAceleration, deltaTime, &aux);
	addVectorLG(&aux, &phy->rotationSpeed);
	multiplyVectorByScalarGL(&phy->rotationSpeed, deltaTime, &aux);
	addVectorLG(&aux, &phy->rotation);
}

__kernel void update(__global struct PhysicsComponent_ocl_t* phy, __constant float* deltaTime) {
	int i = get_global_id(0);
	if (phy->gravity) {
		aplyGravity(&phy[i], *deltaTime);
	}
	moveObject(&phy[i], *deltaTime);
}