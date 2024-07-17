#include <game/ocl/mwc64x_rng.cl>

//#pragma OPENCL EXTENSION cl_khr_fp64 : enable

//#pragma OPENCL EXTENSION cl_khr_fp64 : enable

//CONSTANTS
#define ERRORSIM 1
#define OKSIM    0
#define PRECISION 1e3
#define MINVARVALUE -50
#define MAXVARVALUE 50
#define RAND_MAX UINT_MAX
#define INDEX(i, j, v) i*(v+1)+j 
#define VALUE(i, v) INDEX(i, v, v)
#define MAX_WORK_SIZE 256
#define RFPOS(i, c) (i<<2)+c //i*4+c
#define RBPOS(i, c, v) (i<<2)+c+v //i*4+c
#define MIN_X 0
#define MIN_Y 1
#define MAX_X 2
#define MAX_Y 3
#define POPULATION 255

//OBJECTIVE FUNCTION
float MyObjective(__global float* x, int VARS)
{
    int idx = get_local_id(0);
    __global float* vars = &x[idx * (VARS + 1)];

    float f = 0.0f;// Evaluations++;
    float rect1[4] = { FLT_MAX, FLT_MAX, 0, 0 };  //rect = ({minx, miny}, {maxx, maxy})
    //Calculate maximum and minimum point in both axis for team 1. Define a square with those points
    for (size_t i = 0; i < VARS / 2; i += 2) {  //Each pj has x and y, so two variables are used in each iteration
        float Px = vars[i];
        float Py = vars[i + 1];

        if (Px < rect1[MIN_X]) {
            rect1[MIN_X] = Px;
        }
        else if (Px > rect1[MAX_X]) {
            rect1[MAX_X] = Px;
        }

        if (Py < rect1[MIN_Y]) {
            rect1[MIN_Y] = Py;
        }
        else if (Py > rect1[MAX_Y]) {
            rect1[MAX_Y] = Py;
        }
    }

    //Calculate maximum and minimum point in both axis for team 2. Define a square with those points
    float rect2[4] = { FLT_MAX, FLT_MAX, 0, 0 };  //rect = ({minx, miny}, {maxx, maxy}) 
    for (size_t i = VARS / 2; i < VARS; i += 2) {  //Each pj has x and y, so two variables are used in each iteration
        float Px = vars[i];
        float Py = vars[i + 1];

        if (Px < rect2[MIN_X]) {
            rect2[MIN_X] = Px;
        }
        else if (Px > rect2[MAX_X]) {
            rect2[MAX_X] = Px;
        }

        if (Py < rect2[MIN_Y]) {
            rect2[MIN_Y] = Py;
        }
        else if (Py > rect2[MAX_Y]) {
            rect2[MAX_Y] = Py;
        }
    }

    //Formula to calculate the sides of the square formed by the intersection of both formation squares
    float dx = fmin(rect1[MAX_X], rect2[MAX_X]) - fmax(rect1[MIN_X], rect2[MIN_X]);
    float dy = fmin(rect1[MAX_Y], rect2[MAX_Y]) - fmax(rect1[MIN_Y], rect2[MIN_Y]);

    if (dx <= 0 || dy <= 0) {
        f = -1;
    }
    else {
        f = dx * dy;  //Area of the square = side * side
    }

    return f;
}

/*============================================================================ */
/* Random Functions: Generates a random sequence with a seed */
/*============================================================================ */
void seedRandomSecuence(int i, int vars, ulong seed, mwc64x_state_t* state) {
    MWC64X_SeedStreams(state, i * vars, vars);
    MWC64X_Skip(state, seed * get_global_size(0));
}

/*============================================================================ */
/* Random Functions: generates values for the indvidual variables  */
/*============================================================================ */
float var_rand(mwc64x_state_t* state) {
    return MINVARVALUE + (MAXVARVALUE - MINVARVALUE) * (float)MWC64X_NextUint(state) / ((float)RAND_MAX);
}

/*================================================================================ */
/* Random Functions: generates values for the Jaya algorithm coeficients  */
/*================================================================================ */
float coef_rand(mwc64x_state_t* state)
{
    return (float)MWC64X_NextUint(state) / (float)RAND_MAX;
}


void getMinMax(__global float* x, __global float* maxVal, __global float* minVal, __global int* imax, __global int* imin, int vars,
    __local float* evalMax, __local float* evalMin, __local int* evalMaxIndex, __local int* evalMinIndex) {
    int i = get_local_id(0);
    barrier(CLK_LOCAL_MEM_FENCE);
    for (int n = 2; n <= POPULATION; n *= 2) {
        if ((i & n - 1) == 0) {  //i % n == 0
            int offset = n >> 1; // n / 2
            if (evalMin[i] > evalMin[i + offset]) {
                evalMin[i] = evalMin[i + offset];
                evalMinIndex[i] = evalMinIndex[i + offset];
            }
            if (evalMax[i] < evalMax[i + offset]) {
                evalMax[i] = evalMax[i + offset];
                evalMaxIndex[i] = evalMaxIndex[i + offset];
            }
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if (i == 0) {
        *maxVal = evalMax[0];
        *minVal = evalMin[0];
        *imax = evalMaxIndex[0];
        *imin = evalMinIndex[0];
    }
}

/*=============================================================================== */
/* Create population, and select best and worst (max and min)  */
/*=============================================================================== */
void createPopulation(__global float* x, __global float* maxVal, __global float* minVal, __global int* imax, __global int* imin, ulong seed, int vars,
    mwc64x_state_t* state, __local float* evalMax, __local float* evalMin, __local int* evalMaxIndex, __local int* evalMinIndex) {
    int i = get_local_id(0);
    seedRandomSecuence(i, vars, seed, state);

    for (int j = 0; j < vars; j++) {
        float rnd = var_rand(state);
        x[INDEX(i, j, vars)] = rnd;
    }

    //Evaluate each individual
    float res = MyObjective(x, vars);
    x[VALUE(i, vars)] = res;
    evalMin[i] = res;
    evalMax[i] = res;
    evalMinIndex[i] = i;
    evalMaxIndex[i] = i;

    getMinMax(x, maxVal, minVal, imax, imin, vars, evalMax,
        evalMin, evalMaxIndex, evalMinIndex);
}

/*============================================================================ */
/* Update the variables of the population using the best and worst individuals */
/*============================================================================ */
void updatePopulation(__global float* x, __global float* maxVal, __global float* minVal, __global int* imax, __global int* imin, ulong seed, int vars,
    mwc64x_state_t* state, __local float* evalMax, __local float* evalMin, __local int* evalMaxIndex, __local int* evalMinIndex, __global float* xn) {
    __global float* xnew = xn; // xn - 1; TODO QUE HACIA ESTO!!!!!??????
    int i = get_local_id(0);
    // A new individual is created
    // xnew(i, j) = x(i, j) + r(1)*(Best(j) - abs(x(i, j))) - r(2)*(worst(j) - abs(x(i, j)));
    for (int j = 0; j < vars; j++) {
        xnew[INDEX(i, j, vars)] = x[INDEX(i, j, vars)] + coef_rand(state) * (x[INDEX(*imin, j, vars)] - fabs(x[INDEX(i, j, vars)]))
            - coef_rand(state) * (x[INDEX(*imax, j, vars)] - fabs(x[INDEX(i, j, vars)]));
    }

    float res = MyObjective(xnew, vars);
    if (res < x[VALUE(i, vars)]) {
        // Update variables
        for (int j = 0; j < vars; j++) {
            x[INDEX(i, j, vars)] = xnew[INDEX(i, j, vars)];
        }
        x[VALUE(i, vars)] = res;
    }

    evalMin[i] = x[VALUE(i, vars)];
    evalMax[i] = x[VALUE(i, vars)];
    evalMinIndex[i] = i;
    evalMaxIndex[i] = i;
    getMinMax(x, maxVal, minVal, imax, imin, vars, evalMax,
        evalMin, evalMaxIndex, evalMinIndex);
}

/*
*   Gets the minimum value of all the runs
*/
__kernel void getMinResult(__global float* glob_minVal, __global int* glob_imin, __global int* s_imin, __global int* s_rmin, int vars, int iterations, int population, int runs) {
    if (get_global_id(0) == 0) { // This trivial work will do it only one thread. It is done on the device to not copy data back to the CPU RAM
        int min = glob_minVal[0];
        int i_min = glob_imin[0];
        int r_min = 0;

        for (int i = 0; i < runs; i++) {
            if (glob_minVal[i] < min) {
                min = glob_minVal[i];
                i_min = glob_imin[i];
                r_min = i;
            }
        }
        *s_imin = i_min;
        *s_rmin = r_min;
    }
}

__kernel void jayaGPU(__global float* glob_x, __global float* glob_maxVal, __global float* glob_minVal, __global int* glob_imax, __global int* glob_imin,
    int vars, ulong seed, int iterations, __local float* evalMax, __local float* evalMin, __local int* evalMaxIndex, __local int* evalMinIndex, __global float* glob_xnew) {
    int iter = 0;
    mwc64x_state_t state;

    int local_id = get_local_id(0);
    int global_id = get_global_id(0);
    int group_id = get_group_id(0);
    int i = local_id;

    // Set global pointers to the global memory space for this run
    __global float* x = &glob_x[group_id * 256 * vars];
    __global float* maxVal = &glob_maxVal[group_id];
    __global float* minVal = &glob_minVal[group_id];
    __global int* imax = &glob_imax[group_id];
    __global int* imin = &glob_imin[group_id];
    __global float* xnew = &glob_xnew[group_id * 256 * vars];

    createPopulation(x, maxVal, minVal, imax, imin, seed, vars,
        &state, evalMax, evalMin, evalMaxIndex, evalMinIndex);
    while (iter < iterations) {
        updatePopulation(x, maxVal, minVal, imax, imin, seed, vars,
            &state, evalMax, evalMin, evalMaxIndex, evalMinIndex, xnew);
        iter++;
    }
}