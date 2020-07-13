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
    struct mivec3_t speed;
    struct mivec3_t rotationAceleration;
    struct mivec3_t rotationSpeed;
    struct mivec3_t scale;
    struct mivec3_t gravityAcell;
    struct mivec3_t aceleration;
    bool gravity;
};

struct Entity_ocl_t {
    struct PhysicsComponent_ocl_t;
    //ETC
};

/*
    Multiplies vector a by b
*//*
void multiplyVectors(mivec3 a, mivec3 b) {
    a.x *
}*/

/*
    Copies vector source to target
*/
void copyVector(__global struct mivec3_t* source, __global struct mivec3_t* target) {
    source->x = target->x;
    source->y = target->y;
    source->z = target->z;
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