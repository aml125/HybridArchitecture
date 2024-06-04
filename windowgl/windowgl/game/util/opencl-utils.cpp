#include <game/util/opencl-utils.hpp>
#include <iostream>

//we want to use POSIX functions
#pragma warning( push )
#pragma warning( disable : 4996 )


namespace GM {

    /* This function helps to create informative messages in
     * case when OpenCL errors occur. It returns a string
     * representation for an OpenCL error code.
     * (E.g. "CL_DEVICE_NOT_FOUND" instead of just -1.)
     */
    const char* TranslateOpenCLError(cl_int errorCode)
    {
        switch (errorCode)
        {
        case CL_SUCCESS:                            return "CL_SUCCESS";
        case CL_DEVICE_NOT_FOUND:                   return "CL_DEVICE_NOT_FOUND";
        case CL_DEVICE_NOT_AVAILABLE:               return "CL_DEVICE_NOT_AVAILABLE";
        case CL_COMPILER_NOT_AVAILABLE:             return "CL_COMPILER_NOT_AVAILABLE";
        case CL_MEM_OBJECT_ALLOCATION_FAILURE:      return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
        case CL_OUT_OF_RESOURCES:                   return "CL_OUT_OF_RESOURCES";
        case CL_OUT_OF_HOST_MEMORY:                 return "CL_OUT_OF_HOST_MEMORY";
        case CL_PROFILING_INFO_NOT_AVAILABLE:       return "CL_PROFILING_INFO_NOT_AVAILABLE";
        case CL_MEM_COPY_OVERLAP:                   return "CL_MEM_COPY_OVERLAP";
        case CL_IMAGE_FORMAT_MISMATCH:              return "CL_IMAGE_FORMAT_MISMATCH";
        case CL_IMAGE_FORMAT_NOT_SUPPORTED:         return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
        case CL_BUILD_PROGRAM_FAILURE:              return "CL_BUILD_PROGRAM_FAILURE";
        case CL_MAP_FAILURE:                        return "CL_MAP_FAILURE";
        case CL_MISALIGNED_SUB_BUFFER_OFFSET:       return "CL_MISALIGNED_SUB_BUFFER_OFFSET";                          //-13
        case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:    return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";   //-14
        case CL_COMPILE_PROGRAM_FAILURE:            return "CL_COMPILE_PROGRAM_FAILURE";                               //-15
        case CL_LINKER_NOT_AVAILABLE:               return "CL_LINKER_NOT_AVAILABLE";                                  //-16
        case CL_LINK_PROGRAM_FAILURE:               return "CL_LINK_PROGRAM_FAILURE";                                  //-17
        case CL_DEVICE_PARTITION_FAILED:            return "CL_DEVICE_PARTITION_FAILED";                               //-18
        case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:      return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";                         //-19
        case CL_INVALID_VALUE:                      return "CL_INVALID_VALUE";
        case CL_INVALID_DEVICE_TYPE:                return "CL_INVALID_DEVICE_TYPE";
        case CL_INVALID_PLATFORM:                   return "CL_INVALID_PLATFORM";
        case CL_INVALID_DEVICE:                     return "CL_INVALID_DEVICE";
        case CL_INVALID_CONTEXT:                    return "CL_INVALID_CONTEXT";
        case CL_INVALID_QUEUE_PROPERTIES:           return "CL_INVALID_QUEUE_PROPERTIES";
        case CL_INVALID_COMMAND_QUEUE:              return "CL_INVALID_COMMAND_QUEUE";
        case CL_INVALID_HOST_PTR:                   return "CL_INVALID_HOST_PTR";
        case CL_INVALID_MEM_OBJECT:                 return "CL_INVALID_MEM_OBJECT";
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:    return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
        case CL_INVALID_IMAGE_SIZE:                 return "CL_INVALID_IMAGE_SIZE";
        case CL_INVALID_SAMPLER:                    return "CL_INVALID_SAMPLER";
        case CL_INVALID_BINARY:                     return "CL_INVALID_BINARY";
        case CL_INVALID_BUILD_OPTIONS:              return "CL_INVALID_BUILD_OPTIONS";
        case CL_INVALID_PROGRAM:                    return "CL_INVALID_PROGRAM";
        case CL_INVALID_PROGRAM_EXECUTABLE:         return "CL_INVALID_PROGRAM_EXECUTABLE";
        case CL_INVALID_KERNEL_NAME:                return "CL_INVALID_KERNEL_NAME";
        case CL_INVALID_KERNEL_DEFINITION:          return "CL_INVALID_KERNEL_DEFINITION";
        case CL_INVALID_KERNEL:                     return "CL_INVALID_KERNEL";
        case CL_INVALID_ARG_INDEX:                  return "CL_INVALID_ARG_INDEX";
        case CL_INVALID_ARG_VALUE:                  return "CL_INVALID_ARG_VALUE";
        case CL_INVALID_ARG_SIZE:                   return "CL_INVALID_ARG_SIZE";
        case CL_INVALID_KERNEL_ARGS:                return "CL_INVALID_KERNEL_ARGS";
        case CL_INVALID_WORK_DIMENSION:             return "CL_INVALID_WORK_DIMENSION";
        case CL_INVALID_WORK_GROUP_SIZE:            return "CL_INVALID_WORK_GROUP_SIZE";
        case CL_INVALID_WORK_ITEM_SIZE:             return "CL_INVALID_WORK_ITEM_SIZE";
        case CL_INVALID_GLOBAL_OFFSET:              return "CL_INVALID_GLOBAL_OFFSET";
        case CL_INVALID_EVENT_WAIT_LIST:            return "CL_INVALID_EVENT_WAIT_LIST";
        case CL_INVALID_EVENT:                      return "CL_INVALID_EVENT";
        case CL_INVALID_OPERATION:                  return "CL_INVALID_OPERATION";
        case CL_INVALID_GL_OBJECT:                  return "CL_INVALID_GL_OBJECT";
        case CL_INVALID_BUFFER_SIZE:                return "CL_INVALID_BUFFER_SIZE";
        case CL_INVALID_MIP_LEVEL:                  return "CL_INVALID_MIP_LEVEL";
        case CL_INVALID_GLOBAL_WORK_SIZE:           return "CL_INVALID_GLOBAL_WORK_SIZE";                           //-63
        case CL_INVALID_PROPERTY:                   return "CL_INVALID_PROPERTY";                                   //-64
        case CL_INVALID_IMAGE_DESCRIPTOR:           return "CL_INVALID_IMAGE_DESCRIPTOR";                           //-65
        case CL_INVALID_COMPILER_OPTIONS:           return "CL_INVALID_COMPILER_OPTIONS";                           //-66
        case CL_INVALID_LINKER_OPTIONS:             return "CL_INVALID_LINKER_OPTIONS";                             //-67
        case CL_INVALID_DEVICE_PARTITION_COUNT:     return "CL_INVALID_DEVICE_PARTITION_COUNT";                     //-68
    //    case CL_INVALID_PIPE_SIZE:                  return "CL_INVALID_PIPE_SIZE";                                  //-69
    //    case CL_INVALID_DEVICE_QUEUE:               return "CL_INVALID_DEVICE_QUEUE";                               //-70    

        default:
            return "UNKNOWN ERROR CODE";
        }
    }

    ocl_args_d_t::ocl_args_d_t(std::string& device, bool GPU) :
        context(NULL),
        device(NULL),
        commandQueue(NULL),
        platformVersion(OPENCL_VERSION_1_2),
        deviceVersion(OPENCL_VERSION_1_2),
        compilerVersion(OPENCL_VERSION_1_2)
    {
        this->GPU = GPU;
        //Names: NVIDIA CUDA, Intel
        setupOpenCL(this, device.c_str());
        size_t param_value_size = sizeof(size_t);
        size_t param_value = 0;
        int result = clGetDeviceInfo(
            this->device,
            CL_DEVICE_MAX_WORK_GROUP_SIZE,
            param_value_size,
            (void*)&param_value,
            NULL);

        if (result != CL_SUCCESS) {
            Log::log("ERROR accesing workgoup info: " + std::string(TranslateOpenCLError(result)));
        }
        else {
            Log::log(std::string("Max workgroup size: ") + std::to_string(param_value));
        }
    }

    /*
     * destructor - called only once
     * Release all OpenCL objects
     * This is a regular sequence of calls to deallocate all created OpenCL resources in bootstrapOpenCL.
     *
     * You may want to call these deallocation procedures in the middle of your application execution
     * (not at the end) if you don't further need OpenCL runtime.
     * You may want to do that in order to free some memory, for example,
     * or recreate OpenCL objects with different parameters.
     *
     */
    ocl_args_d_t::~ocl_args_d_t()
    {
        cl_int err = CL_SUCCESS;

        if (commandQueue)
        {
            err = clReleaseCommandQueue(commandQueue);
            if (CL_SUCCESS != err)
            {
                Log::log("Error: clReleaseCommandQueue returned '%s'.\n" + std::string(TranslateOpenCLError(err)));
            }
        }
        if (device)
        {
            err = clReleaseDevice(device);
            if (CL_SUCCESS != err)
            {
                Log::log("Error: clReleaseDevice returned '%s'.\n" + std::string(TranslateOpenCLError(err)));
            }
        }
        if (context)
        {
            err = clReleaseContext(context);
            if (CL_SUCCESS != err)
            {
                Log::log("Error: clReleaseContext returned '%s'.\n" + std::string(TranslateOpenCLError(err)));
            }
        }
        /*
         * Note there is no procedure to deallocate platform
         * because it was not created at the startup,
         * but just queried from OpenCL runtime.
         */
    }

    /*
         * Check whether an OpenCL platform is the required platform
         * (based on the platform's name)
         */
    bool CheckPreferredPlatformMatch(cl_platform_id platform, const char* preferredPlatform)
    {
        size_t stringLength = 0;
        cl_int err = CL_SUCCESS;
        bool match = false;

        // In order to read the platform's name, we first read the platform's name string length (param_value is NULL).
        // The value returned in stringLength
        err = clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, NULL, &stringLength);
        if (CL_SUCCESS != err)
        {
            Log::log("Error: clGetPlatformInfo() to get CL_PLATFORM_NAME length returned '%s'.\n" + std::string(TranslateOpenCLError(err)));
            return false;
        }

        // Now, that we know the platform's name string length, we can allocate enough space before read it
        std::vector<char> platformName(stringLength);

        // Read the platform's name string
        // The read value returned in platformName
        err = clGetPlatformInfo(platform, CL_PLATFORM_NAME, stringLength, &platformName[0], NULL);
        Log::log("Platform name: " + std::string(&platformName[0]) + ". Prefered name: " + std::string(preferredPlatform) + "\n");
        if (CL_SUCCESS != err)
        {
            Log::log("Error: clGetplatform_ids() to get CL_PLATFORM_NAME returned %s.\n" + std::string(TranslateOpenCLError(err)));
            return false;
        }

        // Now check if the platform's name is the required one
        if (strstr(&platformName[0], preferredPlatform) != 0)
        {
            // The checked platform is the one we're looking for
            match = true;
        }

        return match;
    }

    /*
         * Find and return the preferred OpenCL platform
         * In case that preferredPlatform is NULL, the ID of the first discovered platform will be returned
         */
    cl_platform_id FindOpenCLPlatform(const char* preferredPlatform, cl_device_type deviceType)
    {
        cl_uint numPlatforms = 0;
        cl_int err = CL_SUCCESS;

        // Get (in numPlatforms) the number of OpenCL platforms available
        // No platform ID will be return, since platforms is NULL
        err = clGetPlatformIDs(0, NULL, &numPlatforms);
        if (CL_SUCCESS != err)
        {
            Log::log("Error: clGetplatform_ids() to get num platforms returned %s.\n" + std::string(TranslateOpenCLError(err)));
            return NULL;
        }
        Log::log("Number of available platforms: " + std::to_string(numPlatforms) + "\n");

        if (0 == numPlatforms)
        {
            Log::log("Error: No platforms found!\n");
            return NULL;
        }

        std::vector<cl_platform_id> platforms(numPlatforms);

        // Now, obtains a list of numPlatforms OpenCL platforms available
        // The list of platforms available will be returned in platforms
        err = clGetPlatformIDs(numPlatforms, &platforms[0], NULL);
        if (CL_SUCCESS != err)
        {
            Log::log("Error: clGetplatform_ids() to get platforms returned %s.\n" + std::string(TranslateOpenCLError(err)));
            return NULL;
        }

        // Check if one of the available platform matches the preferred requirements
        for (cl_uint i = 0; i < numPlatforms; i++)
        {
            bool match = true;
            cl_uint numDevices = 0;

            // If the preferredPlatform is not NULL then check if platforms[i] is the required one
            // Otherwise, continue the check with platforms[i]
            if ((NULL != preferredPlatform) && (strlen(preferredPlatform) > 0))
            {
                // In case we're looking for a specific platform
                match = CheckPreferredPlatformMatch(platforms[i], preferredPlatform);
            }

            // match is true if the platform's name is the required one or don't care (NULL)
            if (match)
            {
                // Obtains the number of deviceType devices available on platform
                // When the function failed we expect numDevices to be zero.
                // We ignore the function return value since a non-zero error code
                // could happen if this platform doesn't support the specified device type.
                err = clGetDeviceIDs(platforms[i], deviceType, 0, NULL, &numDevices);
                if (CL_SUCCESS != err)
                {
                    Log::log("clGetDeviceIDs() returned %s.\n" + std::string(TranslateOpenCLError(err)));
                }

                if (0 != numDevices)
                {
                    // There is at list one device that answer the requirements
                    return platforms[i];
                }
            }
        }

        return NULL;
    }

    /*
         * This function read the OpenCL platdorm and device versions
         * (using clGetxxxInfo API) and stores it in the ocl structure.
         * Later it will enable us to support both OpenCL 1.2 and 2.0 platforms and devices
         * in the same program.
         */
    int GetPlatformAndDeviceVersion(cl_platform_id platformId, ocl_args_d_t* ocl)
    {
        cl_int err = CL_SUCCESS;

        // Read the platform's version string length (param_value is NULL).
        // The value returned in stringLength
        size_t stringLength = 0;
        err = clGetPlatformInfo(platformId, CL_PLATFORM_VERSION, 0, NULL, &stringLength);
        if (CL_SUCCESS != err)
        {
            Log::log("Error: clGetPlatformInfo() to get CL_PLATFORM_VERSION length returned '%s'.\n" + std::string(TranslateOpenCLError(err)));
            return err;
        }

        // Now, that we know the platform's version string length, we can allocate enough space before read it
        std::vector<char> platformVersion(stringLength);

        // Read the platform's version string
        // The read value returned in platformVersion
        err = clGetPlatformInfo(platformId, CL_PLATFORM_VERSION, stringLength, &platformVersion[0], NULL);
        if (CL_SUCCESS != err)
        {
            Log::log("Error: clGetplatform_ids() to get CL_PLATFORM_VERSION returned %s.\n" + std::string(TranslateOpenCLError(err)));
            return err;
        }

        if (strstr(&platformVersion[0], "OpenCL 2.0") != NULL)
        {
            ocl->platformVersion = OPENCL_VERSION_2_0;
        }

        //DEVICE NAME
        //Read de device's name string length
        err = clGetDeviceInfo(ocl->device, CL_DEVICE_NAME, 0, NULL, &stringLength);
        if (CL_SUCCESS != err)
        {
            Log::log("Error: clGetDeviceInfo() to get CL_DEVICE_NAME length returned '%s'.\n" + std::string(TranslateOpenCLError(err)));
            return err;
        }

        // Now, that we know the device's name string length, we can allocate enough space before read it
        std::vector<char> deviceName(stringLength);

        // Read the device's version string
        // The read value returned in deviceVersion
        err = clGetDeviceInfo(ocl->device, CL_DEVICE_NAME, stringLength, &deviceName[0], NULL);
        if (CL_SUCCESS != err)
        {
            Log::log("Error: clGetDeviceInfo() to get CL_DEVICE_NAME returned %s.\n" + std::string(TranslateOpenCLError(err)));
            return err;
        }

        Log::log("Device choosen: \n" + std::string(&deviceName[0]));

        // Read the device's version string length (param_value is NULL).
        err = clGetDeviceInfo(ocl->device, CL_DEVICE_VERSION, 0, NULL, &stringLength);
        if (CL_SUCCESS != err)
        {
            Log::log("Error: clGetDeviceInfo() to get CL_DEVICE_VERSION length returned '%s'.\n" + std::string(TranslateOpenCLError(err)));
            return err;
        }

        // Now, that we know the device's version string length, we can allocate enough space before read it
        std::vector<char> deviceVersion(stringLength);

        // Read the device's version string
        // The read value returned in deviceVersion
        err = clGetDeviceInfo(ocl->device, CL_DEVICE_VERSION, stringLength, &deviceVersion[0], NULL);
        if (CL_SUCCESS != err)
        {
            Log::log("Error: clGetDeviceInfo() to get CL_DEVICE_VERSION returned %s.\n" + std::string(TranslateOpenCLError(err)));
            return err;
        }

        if (strstr(&deviceVersion[0], "OpenCL 2.0") != NULL)
        {
            ocl->deviceVersion = OPENCL_VERSION_2_0;
        }

        // Read the device's OpenCL C version string length (param_value is NULL).
        err = clGetDeviceInfo(ocl->device, CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &stringLength);
        if (CL_SUCCESS != err)
        {
            Log::log("Error: clGetDeviceInfo() to get CL_DEVICE_OPENCL_C_VERSION length returned '%s'.\n" + std::string(TranslateOpenCLError(err)));
            return err;
        }

        // Now, that we know the device's OpenCL C version string length, we can allocate enough space before read it
        std::vector<char> compilerVersion(stringLength);

        // Read the device's OpenCL C version string
        // The read value returned in compilerVersion
        err = clGetDeviceInfo(ocl->device, CL_DEVICE_OPENCL_C_VERSION, stringLength, &compilerVersion[0], NULL);
        if (CL_SUCCESS != err)
        {
            Log::log("Error: clGetDeviceInfo() to get CL_DEVICE_OPENCL_C_VERSION returned %s.\n" + std::string(TranslateOpenCLError(err)));
            return err;
        }

        else if (strstr(&compilerVersion[0], "OpenCL C 2.0") != NULL)
        {
            ocl->compilerVersion = OPENCL_VERSION_2_0;
        }

        return err;
    }

    /*
         * This function picks/creates necessary OpenCL objects which are needed.
         * The objects are:
         * OpenCL platform, device, context, and command queue.
         *
         * All these steps are needed to be performed once in a regular OpenCL application.
         * This happens before actual compute kernels calls are performed.
         *
         * For convenience, in this application you store all those basic OpenCL objects in structure ocl_args_d_t,
         * so this function populates fields of this structure, which is passed as parameter ocl.
         * Please, consider reviewing the fields before going further.
         * The structure definition is right in the beginning of this file.
         */
    int setupOpenCL(ocl_args_d_t* ocl, const std::string& platformName)
    {
        cl_device_type deviceType = CL_DEVICE_TYPE_GPU;
        if (!ocl->GPU) {
            deviceType = CL_DEVICE_TYPE_CPU;
        }

        // The following variable stores return codes for all OpenCL calls.
        cl_int err = CL_SUCCESS;

        // Query for all available OpenCL platforms on the system
        // Here you enumerate all platforms and pick one which name has preferredPlatform as a sub-string
        cl_platform_id platformId = FindOpenCLPlatform(platformName.c_str(), deviceType);
        if (NULL == platformId)
        {
            Log::log("Error: Failed to find OpenCL platform.\n");
            return CL_INVALID_VALUE;
        }

        // Create context with device of specified type.
        // Required device type is passed as function argument deviceType.
        // So you may use this function to create context for any CPU or GPU OpenCL device.
        // The creation is synchronized (pfn_notify is NULL) and NULL user_data
        cl_context_properties contextProperties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platformId, 0 };
        ocl->context = clCreateContextFromType(contextProperties, deviceType, NULL, NULL, &err);
        if ((CL_SUCCESS != err) || (NULL == ocl->context))
        {
            Log::log("Couldn't create a context, clCreateContextFromType() returned '%s'.\n" + std::string(TranslateOpenCLError(err)));
            return err;
        }

        // Query for OpenCL device which was used for context creation
        cl_device_id devices[2]; // Shortcut to only get the first device of the platform
        err = clGetContextInfo(ocl->context, CL_CONTEXT_DEVICES, sizeof(cl_device_id)*2, devices, NULL);
        if (CL_SUCCESS != err)
        {
            Log::log("Error: clGetContextInfo() to get list of devices returned %s.\n" + std::string(TranslateOpenCLError(err)));
            return err;
        }
        ocl->device = devices[0];

        // Read the OpenCL platform's version and the device OpenCL and OpenCL C versions
        GetPlatformAndDeviceVersion(platformId, ocl);

        // Create command queue.
        // OpenCL kernels are enqueued for execution to a particular device through special objects called command queues.
        // Command queue guarantees some ordering between calls and other OpenCL commands.
        // Here you create a simple in-order OpenCL command queue that doesn't allow execution of two kernels in parallel on a target device.
#ifdef CL_VERSION_2_0

        if (OPENCL_VERSION_2_0 == ocl->deviceVersion)
        {
            const cl_command_queue_properties properties[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0 };
            ocl->commandQueue = clCreateCommandQueueWithProperties(ocl->context, ocl->device, properties, &err);
        }
        else {
            // default behavior: OpenCL 1.2
            cl_command_queue_properties properties = CL_QUEUE_PROFILING_ENABLE;
            ocl->commandQueue = clCreateCommandQueue(ocl->context, ocl->device, properties, &err);
        }
#else
    // default behavior: OpenCL 1.2
        cl_command_queue_properties properties = CL_QUEUE_PROFILING_ENABLE;
        ocl->commandQueue = clCreateCommandQueue(ocl->context, ocl->device, properties, &err);
#endif
        if (CL_SUCCESS != err)
        {
            Log::log("Error: clCreateCommandQueue() returned %s.\n" + std::string(TranslateOpenCLError(err)));
            return err;
        }

        return CL_SUCCESS;
    }

    // Upload the OpenCL C source code to output argument source
        // The memory resource is implicitly allocated in the function
        // and should be deallocated by the caller
    int ReadSourceFromFile(const char* fileName, char** source, size_t* sourceSize)
    {
        int errorCode = CL_SUCCESS;

        FILE* fp = NULL;
        fopen_s(&fp, fileName, "rb");
        if (fp == NULL)
        {
            Log::log("Error: Couldn't find program source file '%s'.\n" + std::string(fileName));
            errorCode = CL_INVALID_VALUE;
        }
        else {
            fseek(fp, 0, SEEK_END);
            *sourceSize = ftell(fp);
            fseek(fp, 0, SEEK_SET);

            *source = new char[*sourceSize];
            if (*source == NULL)
            {
                Log::log("Error: Couldn't allocate %d bytes for program source from file '%s'.\n" + std::to_string(*sourceSize) + std::string(fileName));
                errorCode = CL_OUT_OF_HOST_MEMORY;
            }
            else {
                fread(*source, 1, *sourceSize, fp);
            }
        }
        return errorCode;
    }

    /*
         * Create and build OpenCL program from its source code
         */
    int createAndBuildProgram(ocl_args_d_t* ocl, cl_program& program, const std::string& kernelFile)
    {
        cl_int err = CL_SUCCESS;

        // Upload the OpenCL C source code from the input file to source
        // The size of the C program is returned in sourceSize
        char* source = NULL;
        size_t src_size = 0;
        err = ReadSourceFromFile(kernelFile.c_str(), &source, &src_size);
        if (CL_SUCCESS != err)
        {
            Log::log("Error: ReadSourceFromFile returned %s.\n" + std::string(TranslateOpenCLError(err)));
            goto Finish;
        }

        // And now after you obtained a regular C string call clCreateProgramWithSource to create OpenCL program object.
        program = clCreateProgramWithSource(ocl->context, 1, (const char**)&source, &src_size, &err);
        if (CL_SUCCESS != err)
        {
            Log::log("Error: clCreateProgramWithSource returned %s.\n" + std::string(TranslateOpenCLError(err)));
            goto Finish;
        }

        // Build the program
        // During creation a program is not built. You need to explicitly call build function.
        // Here you just use create-build sequence,
        // but there are also other possibilities when program consist of several parts,
        // some of which are libraries, and you may want to consider using clCompileProgram and clLinkProgram as
        // alternatives.
        err = clBuildProgram(program, 1, &ocl->device, "", NULL, NULL);
        if (CL_SUCCESS != err)
        {
            Log::log("Error: clBuildProgram() for source program returned %s.\n" + std::string(TranslateOpenCLError(err)));

            // In case of error print the build log to the standard output
            // First check the size of the log
            // Then allocate the memory and obtain the log from the program
            if (err == CL_BUILD_PROGRAM_FAILURE)
            {
                size_t log_size = 0;
                clGetProgramBuildInfo(program, ocl->device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

                std::vector<char> build_log(log_size);
                clGetProgramBuildInfo(program, ocl->device, CL_PROGRAM_BUILD_LOG, log_size, &build_log[0], NULL);

                Log::log("Error happened during the build of OpenCL program.\nBuild log:%s" + std::string(&build_log[0]));
            }
        }

    Finish:
        if (source)
        {
            delete[] source;
            source = NULL;
        }

        return err;
    }

    /*
        Creates a kernel from a named function on the .cl file
    */
    void createKernelFromProgram(ocl_args_d_t& ocl, cl_program& program, cl_kernel& kernel, const std::string& functionName) {
        cl_int error_ret;
        kernel = clCreateKernel(program, functionName.c_str(), &error_ret);
        if (error_ret != CL_SUCCESS) {
            Log::log("Error, no se ha podido crear el kernel del programa: " + std::string(TranslateOpenCLError(error_ret)));
            exit(-1);
        }
    }

    /*
        Execute the kernel. Must have set all parameters before
    */
    void executeKernel(const ocl_args_d_t& ocl, cl_kernel& kernel, const unsigned int dimensions, unsigned int* globalDimensionSizes, unsigned int* localDimensionSizes) {
        /*Step 10: Running the kernel.*/
        //Setup global work sizes for every dimension
        if (globalDimensionSizes == nullptr) {
            Log::log("Error: Failed to run kernel, globalDimensionSizes is null");
            exit(-1);
        }
        size_t* global_work_size = new size_t[dimensions];
        for (size_t i = 0; i < dimensions; i++)
        {
            global_work_size[i] = globalDimensionSizes[i];
        }

        //Setup local work sizes for every dimension
        size_t* local_work_size = NULL;
        if (localDimensionSizes != nullptr) {
            local_work_size = new size_t[dimensions];
            for (size_t i = 0; i < dimensions; i++)
            {
                local_work_size[i] = localDimensionSizes[i];
            }
        }

        cl_int status = clEnqueueNDRangeKernel(ocl.commandQueue, kernel, dimensions, NULL,
            global_work_size, local_work_size, 0, NULL, NULL);

        // Free dimension arrays
        //free(global_work_size);
        //free(local_work_size);

        if (CL_SUCCESS != status)
        {
            Log::log("Error: Failed to run kernel, return %s\n" + std::string(TranslateOpenCLError(status)));
            exit(-1);
        }
    }

    /*
        Copies the value to memory and sets the argument
    */
    void copyFloatParam(ocl_args_d_t& ocl, cl_kernel& kernel, unsigned int argumentIndex, cl_mem& buffer, float& value) {
        cl_int status = clEnqueueWriteBuffer(ocl.commandQueue, buffer, true, 0, sizeof(float), &value, NULL, NULL, NULL);
        if (CL_SUCCESS != status)
        {
            Log::log("error: Failed to set argument buffer, returned %s\n" + std::string(TranslateOpenCLError(status)));
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
        Creates a float argument for the kernel
    */
    cl_mem createFloatParam(ocl_args_d_t& ocl, float& value) {
        cl_mem newBuffer = clCreateBuffer(ocl.context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(float), (void*)&value, NULL);
        return newBuffer;
    }

    /*
    * Allocates local memory of defined size
    */
    void allocateLocalMemory(ocl_args_d_t& ocl, cl_kernel& kernel, unsigned int argumentIndex, size_t size) {
        cl_int status = clSetKernelArg(kernel, argumentIndex, size, NULL);
        if (CL_SUCCESS != status)
        {
            Log::log("error: Failed to set argument buffer, returned %s\n" + std::string(TranslateOpenCLError(status)));
            exit(-1);
        }
    }
#pragma warning ( pop )
}
#pragma warning ( pop )