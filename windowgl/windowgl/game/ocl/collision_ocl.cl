#include <game/ocl/ocl_structs.cl>

/*
	True if the two lines overlap
*/
bool linearOverlap(float x1, float w1, float x2, float w2) {
	if ((x1 + w1 >= x2 && x1 + w1 <= x2 + w2) || (x2 + w2 >= x1 && x2 + w2 <= x1 + w1)) {
		return true;
	}
	return false;
}

/*
	Ads the offset to the position
*/
struct mivec3_t calculatePosition(struct mivec3_t* position, __constant struct mivec3_t* length, __constant struct mivec3_t* offset) {
	struct mivec3_t result;
	result.x = position->x - (length->x / 2) + offset->x;
	result.y = position->y - (length->y / 2) + offset->y;  // Length / 2 porque asi se define el origen en el centro de la caja,
	result.z = position->z - (length->z / 2) + offset->z;  // por lo que length es el tamaño del lado de la caja
	return result;
}

/*
	Calculates if two entities collide
*/
bool collide(struct AuxPhy_ocl_t* phy1, struct AuxPhy_ocl_t* phy2, 
	__constant struct BoxCollider_ocl_t* c1, __constant struct BoxCollider_ocl_t* c2) {
	//Add offset to the position
	struct mivec3_t pos1 = calculatePosition(&phy1->position, &c1->length, &c1->offset);
	struct mivec3_t pos2 = calculatePosition(&phy2->position, &c2->length, &c2->offset);

	if (linearOverlap(pos1.x, c1->length.x,
		pos2.x, c2->length.x)) {
		if (linearOverlap(pos1.y, c1->length.y,
			pos2.y, c2->length.y)) {
			if (linearOverlap(pos1.z, c1->length.z,
				pos2.z, c2->length.z)) {
				return true;
			}
		}
	}
	return false;
}

/*
	Modifica la velocidad de phy1 comprobando la colisión de phy1 con phy2
*/
void modifySpeedAndVelocityOnCollision(struct AuxPhy_ocl_t* phy1, struct AuxPhy_ocl_t* phy2,
	__constant struct BoxCollider_ocl_t* coll1, __constant struct BoxCollider_ocl_t* coll2, float deltaTime)
{
	bool oneCollides = false;
	//Get back to the original position, before moving
	struct mivec3_t auxVec;
	multiplyVectorByScalarLL(&phy1->speed, deltaTime, &auxVec);
	substractVectorsLL(&phy1->position, &auxVec, &phy1->position);

	//Check X
	/*float auxspeed = phy1->speed.x;
	phy1->position.x += phy1->speed.x;
	if (collide(phy1, phy2, coll1, coll2)) {
		oneCollides = true;
		phy1->speed.x = 0;
	}
	phy1->position.x -= auxspeed;

	//Check Y
	auxspeed = phy1->speed.y;
	phy1->position.y += phy1->speed.y;
	if (collide(phy1, phy2, coll1, coll2)) {
		oneCollides = true;
		phy1->speed.y = 0;
	}
	phy1->position.y -= auxspeed;

	//Check Z
	auxspeed = phy1->speed.z;
	phy1->position.z += phy1->speed.z;
	if (collide(phy1, phy2, coll1, coll2)) {
		oneCollides = true;
		phy1->speed.z = 0;
	}
	phy1->position.z -= auxspeed;

	if (!oneCollides) {*/
		phy1->speed.x = 0;
		phy1->speed.y = 0;
		phy1->speed.z = 0;
	//}

	//Return position to the new positiojn
	multiplyVectorByScalarLL(&phy1->speed, deltaTime, &auxVec);
	addVectorLL(&auxVec, &phy1->position);
}


__kernel void update(__global struct PhysicsComponent_ocl_t* vecPhy, __constant struct BoxCollider_ocl_t* vecBx, __constant ulong* vecIdx, __constant float* deltaTime) {
	unsigned int i = get_global_id(0);
	unsigned int j = get_global_id(1);

	if (j > i) { //From size_t j = i + 1; i < vec.size()... 
		//Copy data to local memory to avoid race conditions
		struct AuxPhy_ocl_t phy1;
		copyPhyGL(&vecPhy[vecIdx[i]], &phy1);

		struct AuxPhy_ocl_t phy2;
		copyPhyGL(&vecPhy[vecIdx[j]], &phy2);

		if (collide(&phy1, &phy2, &vecBx[i], &vecBx[j])) {
			////Check speed correction for obj 1
			struct mivec3_t speed1 = phy1.speed;
			struct mivec3_t speed2 = phy2.speed;

			modifySpeedAndVelocityOnCollision(&phy1, &phy2, &vecBx[i], &vecBx[j], *deltaTime); //Comprobar los dos objetos, por eso se le da la vuelta a los parametros
			modifySpeedAndVelocityOnCollision(&phy2, &phy1, &vecBx[j], &vecBx[i], *deltaTime);

			//Make the objects not move
			struct mivec3_t auxSub;
			multiplyVectorByScalarLL(&speed1, *deltaTime, &auxSub);
			substractVectorsLL(&phy1.position, &auxSub, &phy1.position);

			multiplyVectorByScalarLL(&speed2, *deltaTime, &auxSub);
			substractVectorsLL(&phy2.position, &auxSub, &phy2.position);

			//Copy data back to global
			copyPhyLG(&phy1, &vecPhy[vecIdx[i]]);
			copyPhyLG(&phy2, &vecPhy[vecIdx[j]]);
		}
	}
}