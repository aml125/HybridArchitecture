#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <cstdlib>
#include "timemeasure.h"
#include "log.h"

#define THREADED false

typedef struct {
    double x, y;
} Vec2;

__device__ double rosenbrock(Vec2 X, double a, double b) {
    return pow(a - X.x, (double)2) + b * pow(X.y - pow(X.x, (double)2), (double)2);
}

__device__ Vec2 gradient(Vec2 X, double a, double b) {
    Vec2 result = {
        -2 * a + 4 * b * pow(X.x, (double)3) - 4 * b * X.x * X.y + 2 * X.x,
        2 * b * (X.y - pow(X.x, (double)2))
    };
    return result;
}

__device__ Vec2 gradient_descent(Vec2 X, double a, double b, int iters, double step, int i) {
    double eval = rosenbrock(X, a, b);
    //printf("{%i} Initial Eval: %lf\n", i/2, eval);

    for (size_t i = 0; i < iters; i++)
    {
        Vec2 grad = gradient(X, a, b);
        X.x -= step * grad.x;
        X.y -= step * grad.y;
        eval = rosenbrock(X, a, b);
    }
    //printf("{%i} Eval: %lf\n", i/2, eval);
    return X;
}

__global__ void gradientKernel(double* vars, double* res, int iters)
{
    int x = threadIdx.x;
    int y = blockIdx.x;
    int i = (y * blockDim.x + x) * 2;
    Vec2 X = { vars[i], vars[i + 1] };
    gradient_descent(X, 1, 100, iters, 0.002, i);
    //printf("I'm {%i}. X: %lf, Y: %lf\n", i, X.x, X.y);
}


/*
*====================================================================================
*   HOST CODE
*
*====================================================================================
*/

std::atomic_int32_t threadCounter;
std::mutex mtx;

// Helper function for using CUDA to add vectors in parallel.
cudaError_t gradientDescentWithCuda(double* vars, double* res, int NPOINTS, int ITERATIONS, double* dev_vars, cudaStream_t *stream)
{
    double* dev_res = 0;

    //std::cout << "e" << std::endl;
    auto cudaStatus = cudaMalloc((void**)&dev_res, NPOINTS * sizeof(double));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    // Launch a kernel on the GPU with one thread for each element.
    //std::cout << "holi" << std::endl;
    gradientKernel << <NPOINTS / 100, 100, 1, *stream >> > (dev_vars, dev_res, ITERATIONS);
    //std::cout << "hola" << std::endl;

    //mtx.lock();
    //std::cout << "lanzado " << std::endl;
    //mtx.unlock();

    /*cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
        goto Error;
    }*/

    

    //// Check for any errors launching the kernel
    //cudaStatus = cudaGetLastError();
    //if (cudaStatus != cudaSuccess) {
    //    fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
    //    goto Error;
    //}
    //
    //// cudaDeviceSynchronize waits for the kernel to finish, and returns
    //// any errors encountered during the launch.
    //cudaStatus = cudaDeviceSynchronize();
    //if (cudaStatus != cudaSuccess) {
    //    fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
    //    goto Error;
    //}

    //// Copy output vector from GPU buffer to host memory.
    //cudaStatus = cudaMemcpy(res, dev_res, NPOINTS * sizeof(int), cudaMemcpyDeviceToHost);
    //if (cudaStatus != cudaSuccess) {
    //    fprintf(stderr, "cudaMemcpy failed!");
    //    goto Error;
    //}
//
Error:
    /*mtx.lock();
    int i = ++threadCounter;
    std::cout << "Terminado: " << i << std::endl;
    mtx.unlock();*/
    //cudaFree(dev_res);
    return cudaStatus;
}

cudaError_t gradientDescentWithCudaThreaded(double* vars, double* res, int NPOINTS, int ITERATIONS, double* dev_vars, cudaStream_t* stream)
{
    double* dev_res = 0;

    //std::cout << "e" << std::endl;
    auto cudaStatus = cudaMalloc((void**)&dev_res, NPOINTS * sizeof(double));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    // Launch a kernel on the GPU with one thread for each element.
    //std::cout << "holi" << std::endl;
    gradientKernel << <NPOINTS / 100, 100, 1, *stream >> > (dev_vars, dev_res, ITERATIONS);
    //std::cout << "hola" << std::endl;

    mtx.lock();
    std::cout << "lanzado " << std::endl;
    mtx.unlock();

    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
        goto Error;
    }



    //// Check for any errors launching the kernel
    //cudaStatus = cudaGetLastError();
    //if (cudaStatus != cudaSuccess) {
    //    fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
    //    goto Error;
    //}
    //
    //// cudaDeviceSynchronize waits for the kernel to finish, and returns
    //// any errors encountered during the launch.
    //cudaStatus = cudaDeviceSynchronize();
    //if (cudaStatus != cudaSuccess) {
    //    fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
    //    goto Error;
    //}

    //// Copy output vector from GPU buffer to host memory.
    //cudaStatus = cudaMemcpy(res, dev_res, NPOINTS * sizeof(int), cudaMemcpyDeviceToHost);
    //if (cudaStatus != cudaSuccess) {
    //    fprintf(stderr, "cudaMemcpy failed!");
    //    goto Error;
    //}
//
Error:
    mtx.lock();
    int i = ++threadCounter;
    std::cout << "Terminado: " << i << std::endl;
    mtx.unlock();
    //cudaFree(dev_res);
    return cudaStatus;
}

void launchKernelOnThread(double* vars, double* res, int NPOINTS, int ITERATIONS, double* dev_vars, cudaStream_t* stream) {
    std::thread thread(gradientDescentWithCudaThreaded, vars, res, NPOINTS, ITERATIONS, dev_vars, stream);
    thread.detach();
}

void launchKernel(double* vars, double* res, int NPOINTS, int ITERATIONS, double* dev_vars, cudaStream_t* stream, int i) {
    cudaError_t cudaStatus = gradientDescentWithCuda(vars, res, NPOINTS, ITERATIONS, dev_vars, stream);
    std::cout << "lanzado: " << i <<  std::endl;
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "gradientKernel failed!");
        exit(-1);
    }
}

int launchTest(int NPOINTS, int ITERATIONS, int processes, cudaStream_t *streams) {
    double* vars = new double[NPOINTS * 2];
    double* res = new double[NPOINTS];


    for (size_t i = 0; i < NPOINTS * 2; i++)
    {
        vars[i] = (double)(rand() % 100) / (double)100;
    }

    double* dev_vars = 0;

    cudaError_t cudaStatus;
    //std::cout << "a" << std::endl;
    // Allocate GPU buffers for three vectors (two input, one output)    .
    cudaStatus = cudaMalloc((void**)&dev_vars, NPOINTS * 2 * sizeof(double));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        return -1;
    }
    //std::cout << "i" << std::endl;
    // Copy input vectors from host memory to GPU buffers.
    cudaStatus = cudaMemcpy(dev_vars, vars, NPOINTS * 2 * sizeof(double), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        return -1;
    }

    threadCounter = 0;

    // Add vectors in parallel.
    for (size_t i = 0; i < processes; i++)
    {
        if (THREADED) {
            launchKernelOnThread(vars, res, NPOINTS, ITERATIONS, dev_vars, &streams[i]);
        }
        else {
            launchKernel(vars, res, NPOINTS, ITERATIONS, dev_vars, &streams[i], i);
        }
    }

    if (THREADED) {
        mtx.lock();
        std::cout << "fin" << std::endl;
        mtx.unlock();

        while (threadCounter < processes)
            Sleep(5);
    }
    else {
        std::cout << "fin" << std::endl;
        cudaStatus = cudaDeviceSynchronize();
        if (cudaStatus != cudaSuccess) {
            fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
            return -1;
        }
    }

    

    std::cout << "sincronizado" << std::endl;

    cudaFree(dev_vars);
    

    return 0;
}

#define MAX_PROCESSES 10

int main()
{
    // Choose which GPU to run on, change this on a multi-GPU system.
    cudaError_t cudaStatus = cudaSetDevice(0);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
        return -1;
    }

    TimeMeasure tm;
    TimeMeasure tm2;
    int NPOINTS = 1000;
    int ITERATIONS = 10000;

    cudaStream_t streams[MAX_PROCESSES];
    for (size_t i = 0; i < MAX_PROCESSES; i++)
    {
        cudaStreamCreate(&streams[i]);
    }

    for (size_t i = 0; i <= MAX_PROCESSES; i+=1)
    {
        tm.StartCounter();
        if (launchTest(NPOINTS, ITERATIONS, i, streams) != 0) {
            std::cout << "ERROR" << std::endl;
            return -1;
        }
        double time = tm.GetCounter();

        GM::log("Proceses: " + std::to_string(i) + " Time: " + std::to_string(time));
    }

    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
        return 1;
    }

    // cudaDeviceReset must be called before exiting in order for profiling and
    // tracing tools such as Nsight and Visual Profiler to show complete traces.
    cudaStatus = cudaDeviceReset();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceReset failed!");
        return 1;
    }
    

    GM::flush();



    return 0;
}


