// TODO: Add OpenCL kernel code here.

#include <game/ocl/mwc64x_rng.cl>

//#pragma OPENCL EXTENSION cl_khr_fp64 : enable

//CONSTANTES
#define ERRORSIM 1
#define OKSIM    0
#define PRECISION 1e3
#define MINVARVALUE -50
#define MAXVARVALUE 50
#define RAND_MAX UINT_MAX
#define INDEX(i, j, v) i*(v+1)+j 
#define VALUE(i, v) INDEX(i, v, v)
#define MAX_WORK_SIZE 512
#define RFPOS(i, c) (i<<2)+c //i*4+c
#define RBPOS(i, c, v) (i<<2)+c+v //i*4+c
#define MIN_X 0
#define MIN_Y 1
#define MAX_X 2
#define MAX_Y 3
#define POPULATION 256

//FUNCION OBJETIVO
float MyObjective(__global float* x, int VARS)
{
    int idx = get_global_id(0);
    __global float* vars = &x[idx*(VARS+1)];

    float f = 0.0f;// Evaluations++;
    float rect1[4] = {FLT_MAX, FLT_MAX, 0, 0};  //rect = ({minx, miny}, {maxx, maxy})
    //Calcular punto máximo y mínimo en los dos ejes para el equipo 1. Definir un cuadrado con estos puntos.
	for (size_t i = 0; i < VARS/2; i+=2) {  //Cada personaje tiene x e y, por lo que usa dos variables
        float Px = vars[i];
        float Py = vars[i+1];

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

    //Calcular punto máximo y mínimo en los dos ejes para el equipo 1. Definir un cuadrado con estos puntos.
    float rect2[4] = {FLT_MAX, FLT_MAX, 0, 0};  //rect = ({minx, miny}, {maxx, maxy}) 
    for (size_t i = VARS/2; i < VARS; i+=2) {  //Cada personaje tiene x e y, por lo que usa dos variables
        float Px = vars[i];
        float Py = vars[i+1];

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

    //Formula para el cálculo de los lados del rectangulo formado por la interseccion
    float dx = fmin(rect1[MAX_X], rect2[MAX_X]) - fmax(rect1[MIN_X], rect2[MIN_X]);
    float dy = fmin(rect1[MAX_Y], rect2[MAX_Y]) - fmax(rect1[MIN_Y], rect2[MIN_Y]);

    if (dx <= 0 || dy <= 0) {
        f = -1;
    }
    else {
        f = dx * dy;  //Area del rectangulo formado por la intersección = lado * lado
    }

	return f;
}

/*============================================================================ */
/* Funciones Aleatorias: Genera una secuencia aleatoria  */
/*============================================================================ */
void seedRandomSecuence(int i, int vars, ulong seed, mwc64x_state_t* state) {
	MWC64X_SeedStreams(state, i*vars, vars);
	MWC64X_Skip(state, seed*get_global_size(0));
}

/*============================================================================ */
/* Funciones Aleatorias: generan valores para las variables de cada individuo  */
/*============================================================================ */
float var_rand(mwc64x_state_t* state) {
	return MINVARVALUE + (MAXVARVALUE - MINVARVALUE) * (float)MWC64X_NextUint(state) / ((float)RAND_MAX);
}

/*================================================================================ */
/* Funciones Aleatorias: generan valores para los coeficientes del algoritmo JAYA  */
/*================================================================================ */
float coef_rand(mwc64x_state_t* state)
{
	return (float)MWC64X_NextUint(state) / (float)RAND_MAX;
}


void getMinMax(__global float* x, __global float* maxVal, __global float* minVal, __global int* imax, __global int* imin, int vars,
    __local float* evalMax, __local float* evalMin, __local int* evalMaxIndex, __local int* evalMinIndex) {
    int i = get_global_id(0);
    barrier(CLK_LOCAL_MEM_FENCE);
    for (int n = 2; n <= POPULATION; n*=2) {
        if ((i & n-1) == 0) {  //i % n == 0
            int offset = n >> 1; // n / 2
            if (evalMin[i] > evalMin[i+offset]) {
                evalMin[i] = evalMin[i+offset];
                evalMinIndex[i] = evalMinIndex[i+offset];
			}
            if (evalMax[i] < evalMax[i+offset]) {
                evalMax[i] = evalMax[i+offset];
                evalMaxIndex[i] = evalMaxIndex[i+offset];
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
/* Crea una población de individuos, y selecciona el mejor y el peor (max y min)  */
/*=============================================================================== */
void createPopulation (__global float* x, __global float* maxVal, __global float* minVal, __global int* imax, __global int* imin, ulong seed, int vars,
    mwc64x_state_t* state, __local float* evalMax, __local float* evalMin, __local int* evalMaxIndex, __local int* evalMinIndex ) {
    int i = get_global_id(0);
	seedRandomSecuence(i, vars, seed, state);

    for (int j = 0; j < vars; j++) {
	    float rnd = var_rand(state);
	    x[INDEX(i, j, vars)] = rnd;
    }

    //Evaluar cada individuo
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
/* Actualiza las variables de las poblaciones en funcion del mejor y del peor*/
/*============================================================================ */
void updatePopulation (__global float* x, __global float* maxVal, __global float* minVal, __global int* imax, __global int* imin, ulong seed, int vars,
    mwc64x_state_t* state, __local float* evalMax, __local float* evalMin, __local int* evalMaxIndex, __local int* evalMinIndex, __global float* xn ) {
    __global float* xnew = xn; // xn - 1; TODO QUE HACIA ESTO!!!!!??????
    int i = get_global_id(0);
    // Se crea un nuevo individuo en funcion del algoritmo Jaya
	// xnew(i, j) = x(i, j) + r(1)*(Best(j) - abs(x(i, j))) - r(2)*(worst(j) - abs(x(i, j)));
    for (int j = 0; j < vars; j++) {
        xnew[INDEX(i, j, vars)] = x[INDEX(i, j, vars)] + coef_rand(state)*(x[INDEX(*imin, j, vars)] - fabs(x[INDEX(i, j, vars)]))
            - coef_rand(state)*(x[INDEX(*imax, j, vars)] - fabs(x[INDEX(i, j, vars)]));
    }

    float res = MyObjective(xnew, vars);
    if (res < x[VALUE(i, vars)]) {
        // Actualizar variables
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

__kernel void jayaGPU(__global float* x, __global float* maxVal, __global float* minVal, __global int* imax, __global int* imin,
    int vars, ulong seed, int iterations, __local float* evalMax, __local float* evalMin, __local int* evalMaxIndex, __local int* evalMinIndex, __global float* xnew) {
	int iter = 0;
	mwc64x_state_t state;
	createPopulation(x, maxVal, minVal, imax, imin, seed, vars, 
        &state, evalMax, evalMin, evalMaxIndex, evalMinIndex);
    while (iter < iterations) {
        updatePopulation(x, maxVal, minVal, imax, imin, seed, vars,
            &state, evalMax, evalMin, evalMaxIndex, evalMinIndex, xnew);
        iter++;
	}
}