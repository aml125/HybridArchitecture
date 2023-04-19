#pragma once

#include <CL\cl.h>
#include <string>
#include <vector>
#include <game/util/log.hpp>

namespace GM {
    // Macros for OpenCL versions
#define OPENCL_VERSION_1_2  1.2f
#define OPENCL_VERSION_2_0  2.0f

    const char* TranslateOpenCLError(cl_int errorCode);

    struct ocl_args_d_t
    {
        ocl_args_d_t(std::string& device, bool GPU);
        ~ocl_args_d_t();

        // Regular OpenCL objects:
        cl_context       context;           // hold the context handler
        cl_device_id     device;            // hold the selected device handler
        cl_command_queue commandQueue;      // hold the commands-queue handler
        float            platformVersion;   // hold the OpenCL platform version (default 1.2)
        float            deviceVersion;     // hold the OpenCL device version (default. 1.2)
        float            compilerVersion;   // hold the device OpenCL C version (default. 1.2)
        bool             GPU;
    };


    int setupOpenCL(ocl_args_d_t* ocl, const std::string& platformName);
    int createAndBuildProgram(ocl_args_d_t* ocl, cl_program& program, const std::string& kernelFile);

    /*
        Creates a buffer for vectors
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

    /*
        Creates a buffer for simple types variables and objects
    */
    template <typename T>
    void createSimpleBuffer(ocl_args_d_t& ocl, cl_mem& buffer, bool writeable, const T& data) {
        auto rwOptions = CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR;
        if (writeable) {
            rwOptions = CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR;
        }
        buffer = clCreateBuffer(ocl.context, rwOptions,
            sizeof(T), (void*)&data, NULL);
    }

    /*
        Creates a buffer for matrices
    */
    template <typename T>
    void createMatrixBuffer(ocl_args_d_t& ocl, cl_mem& buffer, bool writeable, T* data, size_t size) {
        auto rwOptions = CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR;
        if (writeable) {
            rwOptions = CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR;
        }
        buffer = clCreateBuffer(ocl.context, rwOptions,
            sizeof(T) * size, (void*)data, NULL);
    }

    /*
        Creates a local parameter
    */
    template <typename T>
    void createLocalParameter(cl_kernel& kernel, unsigned int argumentIndex, size_t numberOfElements) {
        cl_int status = clSetKernelArg(kernel, argumentIndex, sizeof(T) * numberOfElements, NULL);
        if (CL_SUCCESS != status)
        {
            Log::log("error: Failed to set argument buffer, returned %s\n" + std::string(TranslateOpenCLError(status)));
            exit(-1);
        }
    }

    void allocateLocalMemory(ocl_args_d_t& ocl, cl_kernel& kernel, unsigned int argumentIndex, size_t size);

    void createKernelFromProgram(ocl_args_d_t& ocl, cl_program& program, cl_kernel& kernel, const std::string& functionName);

    /*
        Copies the vector data onto the device memory buffer
    */
    template <typename T>
    void copyParameter(ocl_args_d_t& ocl, cl_kernel& kernel, unsigned int argumentIndex, cl_mem& buffer, const std::vector<T>& data) {
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

    /*
        Copies the data onto the device memory buffer
    */
    template <typename T>
    void copySimpleParameter(ocl_args_d_t& ocl, cl_kernel& kernel, unsigned int argumentIndex, T& data) {
        /*cl_int status = clEnqueueWriteBuffer(ocl.commandQueue, buffer, true, 0, sizeof(T), &data, NULL, NULL, NULL);
        if (CL_SUCCESS != status)
        {
            Log::log("error: Failed to copy data to device memory, returned %s\n" + std::string(TranslateOpenCLError(status)));
            exit(-1);
        }*/
        cl_int status = clSetKernelArg(kernel, argumentIndex, sizeof(T), /*(void*)*/&data);
        if (CL_SUCCESS != status)
        {
            Log::log("error: Failed to set argument buffer, returned %s\n" + std::string(TranslateOpenCLError(status)));
            exit(-1);
        }
    }

    /*
        Copies the matrix onto the device memory buffer
    */
    template <typename T>
    void copyMatrixParameter(ocl_args_d_t& ocl, cl_kernel& kernel, unsigned int argumentIndex, cl_mem& buffer, T* data, size_t size) {
        cl_int status = clEnqueueWriteBuffer(ocl.commandQueue, buffer, true, 0, sizeof(T) * size, data, NULL, NULL, NULL);
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

    ///*
    //    Copies the data onto the device memory buffer (const version)
    //*/
    //template <typename T>
    //void copySimpleParameter(ocl_args_d_t& ocl, cl_kernel& kernel, unsigned int argumentIndex, cl_mem& buffer, const T& data) {
    //    cl_int status = clEnqueueWriteBuffer(ocl.commandQueue, buffer, true, 0, sizeof(T), &data, NULL, NULL, NULL);
    //    if (CL_SUCCESS != status)
    //    {
    //        Log::log("error: Failed to copy data to device memory, returned %s\n" + std::string(TranslateOpenCLError(status)));
    //        exit(-1);
    //    }
    //    status = clSetKernelArg(kernel, argumentIndex, sizeof(cl_mem), (void*)&buffer);
    //    if (CL_SUCCESS != status)
    //    {
    //        Log::log("error: Failed to set argument buffer, returned %s\n" + std::string(TranslateOpenCLError(status)));
    //        exit(-1);
    //    }
    //}

    void executeKernel(const ocl_args_d_t& ocl, cl_kernel& kernel, const unsigned int dimensions, unsigned int* globalDimensionSizes, unsigned int* localDimensionSizes);
    const char* TranslateOpenCLError(cl_int errorCode);

    /*
        Retrieve data from buffer after execution
    */
    template <typename T>
    void readVectorBuffer(const ocl_args_d_t& ocl, cl_mem& buffer, std::vector<T>& data) {
        cl_int status = clEnqueueReadBuffer(ocl.commandQueue, buffer, CL_TRUE, 0,
            sizeof(T) * data.size(), data.data(), 0, NULL, NULL);
        if (CL_SUCCESS != status)
        {
            Log::log("Error: Failed to run kernel, return %s\n" + std::string(TranslateOpenCLError(status)));
            exit(-1);
        }
    }

    /*
        Retrieve data from buffer after execution
    */
    template <typename T>
    void readBuffer(const ocl_args_d_t& ocl, cl_mem& buffer, T& data) {
        cl_int status = clEnqueueReadBuffer(ocl.commandQueue, buffer, CL_TRUE, 0,
            sizeof(T), &data, 0, NULL, NULL);
        if (CL_SUCCESS != status)
        {
            Log::log("Error: Failed to run kernel, return %s\n" + std::string(TranslateOpenCLError(status)));
            exit(-1);
        }
    }

    /*
        Retrieve matrix data from buffer after execution
    */
    template <typename T>
    void readMatrixBuffer(const ocl_args_d_t& ocl, cl_mem& buffer, T data, size_t size) {
        cl_int status = clEnqueueReadBuffer(ocl.commandQueue, buffer, CL_TRUE, 0,
            sizeof(T) * size, data, 0, NULL, NULL);
        if (CL_SUCCESS != status)
        {
            Log::log("Error: Failed to run kernel, return %s\n" + std::string(TranslateOpenCLError(status)));
            exit(-1);
        }
    }

    cl_mem createFloatParam(ocl_args_d_t& ocl, float& value);
    void copyFloatParam(ocl_args_d_t& ocl, cl_kernel& kernel, unsigned int argumentIndex, cl_mem& buffer, float& value);
}