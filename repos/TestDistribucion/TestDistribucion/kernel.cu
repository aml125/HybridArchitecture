
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "timemeasure.h"

#include <stdio.h>

__global__ void addKernel(size_t n)
{
    int c = 0;
    for (size_t i = 0; i < n; i++)
    {
        c++;
    }
}

TimeMeasure tm{};
TimeMeasure tm2{};

void DisplayHeader()
{
    const int kb = 1024;
    const int mb = kb * kb;

    int devCount;
    cudaGetDeviceCount(&devCount);
    std::cout << "CUDA Devices: " << std::endl << std::endl;


    for (int dev = 0; dev < devCount; ++dev) {
        cudaSetDevice(dev);
        cudaDeviceProp deviceProp;
        cudaGetDeviceProperties(&deviceProp, dev);

        printf("\nDevice %d: \"%s\"\n", dev, deviceProp.name);

        printf("Mrmp: %i\n", deviceProp.regsPerMultiprocessor);
    }
}

// Helper function for using CUDA to add vectors in parallel.
cudaError_t addWithCudaStreams(size_t n, int times)
{
    int* dev_c = 0;
    cudaError_t cudaStatus;

    // Choose which GPU to run on, change this on a multi-GPU system.
    cudaStatus = cudaSetDevice(0);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
        goto Error;
    }

    cudaStream_t* streams = (cudaStream_t*)malloc(sizeof(cudaStream_t) * times);
    for (size_t i = 0; i < times; i++)
    {
        cudaStreamCreateWithFlags(&streams[i], cudaStreamNonBlocking);
    }

    // Launch a kernel on the GPU with one thread for each element.
    for (int i = 0; i < times; i++) {
        tm2.StartCounter();
        addKernel << <1, 1, 0, streams[i] >> > (n);
        //cudaStatus = cudaDeviceSynchronize();
        double time_ms = tm2.GetCounter();
        printf("Process %i launched. t: %f Seconds\n", i, time_ms / 1000);
    }

    // Check for any errors launching the kernel
    cudaStatus = cudaGetLastError();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
        goto Error;
    }

    // cudaDeviceSynchronize waits for the kernel to finish, and returns
    // any errors encountered during the launch.
    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
        goto Error;
    }

Error:
    cudaFree(dev_c);
    free(streams);

    return cudaStatus;
}


// Helper function for using CUDA to add vectors in parallel.
cudaError_t addWithCudaSec(size_t n, int times)
{
    int* dev_c = 0;
    cudaError_t cudaStatus;

    // Choose which GPU to run on, change this on a multi-GPU system.
    cudaStatus = cudaSetDevice(0);

    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
        goto Error;
    }

    // Launch a kernel on the GPU with one thread for each element.
    for (int i = 0; i < times; i++) {
        tm2.StartCounter();
        addKernel << <1, 1>> > (n);
        //cudaStatus = cudaDeviceSynchronize();
        double time_ms = tm2.GetCounter();
        printf("Process %i launched. t: %f Seconds\n", i, time_ms / 1000);
    }

    // Check for any errors launching the kernel
    cudaStatus = cudaGetLastError();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
        goto Error;
    }

    // cudaDeviceSynchronize waits for the kernel to finish, and returns
    // any errors encountered during the launch.
    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
        goto Error;
    }

Error:
    cudaFree(dev_c);

    return cudaStatus;
}

int main(int argc, char *argv[])
{
    if (argc > 1 && argc < 3) {
        printf("Error, insuficientes argumentos\nUso: %s iteraciones total_kernels\n", argv[0]);
        exit(1);
    }

    DisplayHeader();

    int iters = 20000000;
    int times = 5;

    if (argc > 1) {
        sprintf(argv[1], "%i", &iters);
        sprintf(argv[2], "%i", &times);
    }

    // Add vectors in parallel.
    tm.StartCounter();
    cudaError_t cudaStatus = addWithCudaSec(iters, times);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "addWithCuda failed!");
        return 1;
    }
    double time_ms = tm.GetCounter();
    printf("All processes finished. t: %f Seconds\n", time_ms/1000);

    // cudaDeviceReset must be called before exiting in order for profiling and
    // tracing tools such as Nsight and Visual Profiler to show complete traces.
    cudaStatus = cudaDeviceReset();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceReset failed!");
        return 1;
    }

    printf("He terminado!\n");

    return 0;
}
