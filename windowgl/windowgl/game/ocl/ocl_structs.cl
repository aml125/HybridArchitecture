// TODO: Add OpenCL kernel code here.

struct mivec3_t {
    float x;
    float y;
    float z;
};

struct PhysicsComponent_ocl_t {
    ulong ComponentId;
    ulong entityId;
    struct mivec3_t position;
    struct mivec3_t rotation;
    struct mivec3_t nextPosition;
    struct mivec3_t nextRotation;
    struct mivec3_t speed;
    struct mivec3_t rotationAceleration;
    struct mivec3_t rotationSpeed;
    struct mivec3_t scale;
    struct mivec3_t gravityAcell;
    struct mivec3_t aceleration;
    bool gravity;
};

struct AuxPhy_ocl_t {
    struct mivec3_t position;
    struct mivec3_t speed;
};

struct BoxCollider_ocl_t {
    ulong ComponentId;
    ulong entityId;
    struct mivec3_t length; //Width length height, tamaño del lado de la caja
    struct mivec3_t offset;
};



/*
    Copies vector source to target
*/
void copyVectorGG(__global struct mivec3_t* source, __global struct mivec3_t* target) {
    target->x = source->x;
    target->y = source->y;
    target->z = source->z;
}

/*
    Copies vector source to target
*/
void copyVectorGL(__global struct mivec3_t* source, struct mivec3_t* target) {
    target->x = source->x;
    target->y = source->y;
    target->z = source->z;
}

/*
    Copies vector source to target
*/
void copyVectorLG(struct mivec3_t* source, __global struct mivec3_t* target) {
    target->x = source->x;
    target->y = source->y;
    target->z = source->z;
}

/*
    Copies vector source to target
*/
void copyVectorLL(struct mivec3_t* source, struct mivec3_t* target) {
    target->x = source->x;
    target->y = source->y;
    target->z = source->z;
}

/*
    Substracts vector B to A and puts the result in target
*/
void substractVectorsLL(struct mivec3_t* a, struct mivec3_t* b, struct mivec3_t* target) {
    target->x = a->x - b->x;
    target->y = a->y - b->y;
    target->z = a->z - b->z;
}

/*
    Multiplies vector a by float scalar, and puts the result on the target
*/
void multiplyVectorByScalarGG(__global struct mivec3_t* vec, float scalar, __global struct mivec3_t* target) {
    target->x = vec->x * scalar;
    target->y = vec->y * scalar;
    target->z = vec->z * scalar;
}

/*
    Multiplies vector a by float scalar, and puts the result on the target
*/
void multiplyVectorByScalarGL(__global struct mivec3_t* vec, float scalar, struct mivec3_t* target) {
    target->x = vec->x * scalar;
    target->y = vec->y * scalar;
    target->z = vec->z * scalar;
}

/*
    Multiplies vector a by float scalar, and puts the result on the target
*/
void multiplyVectorByScalarLG(struct mivec3_t* vec, float scalar, __global struct mivec3_t* target) {
    target->x = vec->x * scalar;
    target->y = vec->y * scalar;
    target->z = vec->z * scalar;
}

/*
    Multiplies vector a by float scalar, and puts the result on the target
*/
void multiplyVectorByScalarLL(struct mivec3_t* vec, float scalar, struct mivec3_t* target) {
    target->x = vec->x * scalar;
    target->y = vec->y * scalar;
    target->z = vec->z * scalar;
}

/*
    Adds source vector into target vector
*/
void addVectorGG(__global struct mivec3_t* source, __global struct mivec3_t* target) {
    target->x += source->x;
    target->y += source->y;
    target->z += source->z;
}

/*
    Adds source vector into target vector
*/
void addVectorGL(__global struct mivec3_t* source, struct mivec3_t* target) {
    target->x += source->x;
    target->y += source->y;
    target->z += source->z;
}

/*
    Adds source vector into target vector
*/
void addVectorLG(struct mivec3_t* source, __global struct mivec3_t* target) {
    target->x += source->x;
    target->y += source->y;
    target->z += source->z;
}

/*
    Adds source vector into target vector
*/
void addVectorLL(struct mivec3_t* source,  struct mivec3_t* target) {
    target->x += source->x;
    target->y += source->y;
    target->z += source->z;
}

/*
    Copies BoxCollider_ocl_t source to target
*/
void copyBxGG(__global struct BoxCollider_ocl_t* source, __global struct BoxCollider_ocl_t* target) {
    target->ComponentId = source->ComponentId;
    target->entityId = source->entityId;
    copyVectorGG(&source->length, &target->length);
    copyVectorGG(&source->offset, &target->offset);
}

/*
    Copies BoxCollider_ocl_t source to target
*/
void copyBxGL(__global struct BoxCollider_ocl_t* source, struct BoxCollider_ocl_t* target) {
    target->ComponentId = source->ComponentId;
    target->entityId = source->entityId;
    copyVectorGL(&source->length, &target->length);
    copyVectorGL(&source->offset, &target->offset);
}

/*
    Copies BoxCollider_ocl_t source to target
*/
void copyBxLG(struct BoxCollider_ocl_t* source, __global struct BoxCollider_ocl_t* target) {
    target->ComponentId = source->ComponentId;
    target->entityId = source->entityId;
    copyVectorLG(&source->length, &target->length);
    copyVectorLG(&source->offset, &target->offset);
}

/*
    Copies PhysicsComponent_ocl_t source to target
*/
void copyPhyGG(__global struct PhysicsComponent_ocl_t* source, __global struct PhysicsComponent_ocl_t* target) {
    target->ComponentId = source->ComponentId;
    target->entityId = source->entityId;
    copyVectorGG(&source->position, &target->position);
    copyVectorGG(&source->rotation, &target->rotation);
    copyVectorGG(&source->speed, &target->speed);
    copyVectorGG(&source->rotationAceleration, &target->rotationAceleration);
    copyVectorGG(&source->rotationSpeed, &target->rotationSpeed);
    copyVectorGG(&source->scale, &target->scale);
    copyVectorGG(&source->gravityAcell, &target->gravityAcell);
    copyVectorGG(&source->aceleration, &target->aceleration);
    target->gravity = source->gravity;

}

/*
    Copies PhysicsComponent_ocl_t source to target
*/
void copyPhyGL(__global struct PhysicsComponent_ocl_t* source, struct AuxPhy_ocl_t* target) {
    copyVectorGL(&source->nextPosition, &target->position);
    copyVectorGL(&source->speed, &target->speed);
    //copyVectorGL(&source->scale, &target->scale);

}

/*
    Copies PhysicsComponent_ocl_t source to target
*/
void copyPhyLG(struct AuxPhy_ocl_t* source, __global struct PhysicsComponent_ocl_t* target) {
    copyVectorLG(&source->position, &target->nextPosition);
    copyVectorLG(&source->speed, &target->speed);
}