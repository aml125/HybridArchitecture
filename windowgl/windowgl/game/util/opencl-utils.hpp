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
        float            platformVersion;   // hold the OpenCL platform version (default 1.2)
        float            deviceVersion;     // hold the OpenCL device version (default. 1.2)
        float            compilerVersion;   // hold the device OpenCL C version (default. 1.2)
    };


    int setupOpenCL(ocl_args_d_t* ocl, const std::string& platformName);
    int createAndBuildProgram(ocl_args_d_t* ocl, cl_program& program, const std::string& kernelFile);

    /*
        Creates a link between main memory and device memory for the selected devices
    */
    template <typename T>
    void createBuffer(ocl_args_d_t& ocl, cl_mem& buffer, bool writeable, const std::vector<T>& data) {
        auto rwOptions = CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR;
        if (writeable) {
            rwOptions = CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR;
        }
        buffer = clCreateBuffer(ocl.context, rwOptions,
            data.size() * sizeof(T), (void*)data.data(), NULL);
    }



    void createKernelFromProgram(ocl_args_d_t& ocl, cl_program& program, cl_kernel& kernel, const std::string& functionName);

    /*
        Copies the data onto the device memory
    */
    template <typename T>
    void copyParameters(ocl_args_d_t& ocl, cl_kernel& kernel, unsigned int argumentIndex, cl_mem& buffer, const std::vector<T>& data) {
        cl_int status = clEnqueueWriteBuffer(ocl.commandQueue, buffer, true, 0, data.size() * sizeof(T), data.data(), NULL, NULL, NULL);
        if (CL_SUCCESS != status)
        {
            Log::log("error: Failed to copy data to device memory, returned %s\n" + std::string(TranslateOpenCLError(status)));
            exit(-1);
        }
        status = clSetKernelArg(kernel, argumentIndex, sizeof(cl_mem), (void*)&buffer);
        if (CL_SUCCESS != status)
        {
            Log::log("error: Failed to set argument buffer, returned %s\n" + std::string(TranslateOpenCLError(status)));
            exit(-1);
        }
    }

    void executeKernel(const ocl_args_d_t& ocl, cl_kernel& kernel, const unsigned int dimensions, const unsigned int* dimensionSizes);
    const char* TranslateOpenCLError(cl_int errorCode);

    /*
        Retrieve data from buffer after execution
    */
    template <typename T>
    void readBuffer(const ocl_args_d_t& ocl, cl_mem& buffer, std::vector<T>& data) {
        cl_int status = clEnqueueReadBuffer(ocl.commandQueue, buffer, CL_TRUE, 0,
            sizeof(T) * data.size(), data.data(), 0, NULL, NULL);
        if (CL_SUCCESS != status)
        {
            Log::log("Error: Failed to run kernel, return %s\n" + std::string(TranslateOpenCLError(status)));
            exit(-1);
        }
    }

    cl_mem createFloatParam(ocl_args_d_t& ocl, float& value);
    void copyFloatParam(ocl_args_d_t& ocl, cl_kernel& kernel, unsigned int argumentIndex, cl_mem& buffer, float& value);
}