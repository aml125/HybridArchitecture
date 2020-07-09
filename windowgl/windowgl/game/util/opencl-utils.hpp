#pragma once

#include <CL\cl.h>
#include <string>
#include <vector>
#include <game/util/log.hpp>

namespace GM {
    // Macros for OpenCL versions
#define OPENCL_VERSION_1_2  1.2f
#define OPENCL_VERSION_2_0  2.0f

    struct ocl_args_d_t
    {
        ocl_args_d_t();
        ~ocl_args_d_t();

        // Regular OpenCL objects:
        cl_context       context;           // hold the context handler
        cl_device_id     device;            // hold the selected device handler
        cl_command_queue commandQueue;      // hold the commands-queue handler
        cl_program       program;           // hold the program handler
        cl_kernel        kernel;            // hold the kernel handler
        float            platformVersion;   // hold the OpenCL platform version (default 1.2)
        float            deviceVersion;     // hold the OpenCL device version (default. 1.2)
        float            compilerVersion;   // hold the device OpenCL C version (default. 1.2)

        // Objects that are specific for algorithm implemented in this sample
        cl_mem           buffer;              // hold first source buffer
    };


    int SetupOpenCL(ocl_args_d_t* ocl, const std::string& platformName);
    int CreateAndBuildProgram(ocl_args_d_t* ocl, const std::string& kernelFile);

    /*
        Creates a link between main memory and device memory for the selected devices
    */
    template <typename T>
    void setParameters(ocl_args_d_t& ocl, const std::vector<T>& data) {
        ocl.buffer = clCreateBuffer(ocl.context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
            data.size() * sizeof(T), (void*)data.data(), NULL);
    }

    void createKernelFromProgram(ocl_args_d_t& ocl, const std::string& functionName);
    void copyParameters(ocl_args_d_t& ocl);
    void executeKernel(const ocl_args_d_t& ocl, const unsigned int dataSize);
    const char* TranslateOpenCLError(cl_int errorCode);

    /*
        Retrieve data from buffer after execution
    */
    template <typename T>
    void readBuffer(const ocl_args_d_t& ocl, std::vector<T>& data) {
        cl_int status = clEnqueueReadBuffer(ocl.commandQueue, ocl.buffer, CL_TRUE, 0,
            sizeof(PhysicsComponent_t) * data.size(), data.data(), 0, NULL, NULL);
        if (CL_SUCCESS != status)
        {
            Log::log("Error: Failed to run kernel, return %s\n" + std::string(TranslateOpenCLError(status)));
            exit(-1);
        }
    }
}