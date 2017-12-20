#include "CLManager.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class CLManager::Imp
{
public:
    Imp(){
    };
    ~Imp(){
        cleanup();
    };

    cl_context context = nullptr;
    cl_command_queue command_queue = nullptr;
    cl_device_id device_id = 0;
    std::vector<cl_program> programs;
    std::vector<cl_kernel> kernels;
public:
    void cleanup(){
        if (command_queue != 0)
            clReleaseCommandQueue(command_queue);

        for (auto k :kernels)
            clReleaseKernel(k);

        for (auto p:programs)
            clReleaseProgram(p);

        if (context != 0)
            clReleaseContext(context);
    }
    cl_context CreatContext(){
        const cl_uint num = 9;
        cl_platform_id platform_id[num];
        cl_uint real_num = 0;
        cl_int err = clGetPlatformIDs(num, platform_id, &real_num);
        if (err != CL_SUCCESS || real_num<=0)
        {
            return nullptr;
        }
        cl_context_properties properties[] = {CL_CONTEXT_PLATFORM,(cl_context_properties)platform_id[0],0};
        cl_context context = clCreateContextFromType(properties, CL_DEVICE_TYPE_GPU, nullptr, nullptr, &err);
        if (err != CL_SUCCESS)
        {
            context = clCreateContextFromType(properties, CL_DEVICE_TYPE_CPU, nullptr, nullptr, &err);
            std::cout<<"create gpu contect fail"<<std::endl;
            if (err != CL_SUCCESS)
            {
                std::cout<<"create cpu contect fail"<<std::endl;
                return nullptr;
            }
        }
        return context;
    }
    cl_command_queue CreatCommandQueue(cl_context context, cl_device_id *device_id){
        size_t device_buffer_size = 0;
        int err = 0;
        err = clGetContextInfo(context, CL_CONTEXT_DEVICES,0 , nullptr,&device_buffer_size);
        if (err != CL_SUCCESS)
        {
            std::cout<<"get context info error "<<std::endl;
            return nullptr;
        }
        cl_device_id *devices = new cl_device_id[device_buffer_size/sizeof(cl_device_id)];
        err = clGetContextInfo(context, CL_CONTEXT_DEVICES,device_buffer_size , devices,0);
        if (err != CL_SUCCESS)
        {
            delete []devices;
            std::cout<<"get context info error"<<std::endl;
        }
        cl_command_queue command_queue;
        command_queue = clCreateCommandQueue(context, devices[0], 0, nullptr);
        *device_id = devices[0];
        delete []devices;
        return command_queue;
    }
    cl_program CreatProgam(cl_context context, cl_device_id device_id, const char* cl_filename){
        std::ifstream ifs(cl_filename,std::ios::in);
        if (!ifs.is_open())
        {
            std::cout<<"open cl file fail"<<std::endl;
            return nullptr;
        }
        std::string kernel_string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char >(0));
        auto kernel = kernel_string.c_str();
        cl_program program = clCreateProgramWithSource(context, 1, &kernel, 0, 0);
        if (program == nullptr)
        {
            std::cout<<"create program error"<<std::endl;
            return nullptr;
        }
        int err = clBuildProgram(program, 0, 0, 0, 0, 0);
        if (err != CL_SUCCESS)
        {
            std::cout<<"build program "<<std::endl;
    
            char buildLog[16384];
            clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,
                                  sizeof(buildLog), buildLog, NULL);
    
            std::cerr << "Error in kernel: " << std::endl;
            std::cerr << buildLog;
            clReleaseProgram(program);
    }
    
    return program;
    };
};


CLManager::CLManager() :imp_ptr_(std::make_shared<CLManager::Imp>())
{
}


CLManager::~CLManager()
{
}

bool CLManager::Init()
{
    imp_ptr_->context = imp_ptr_->CreatContext();
    if (imp_ptr_->context == nullptr)
    {
        std::cout<<"create context error"<<std::endl;
        return false;
    }
    imp_ptr_->command_queue = imp_ptr_->CreatCommandQueue(imp_ptr_->context, &imp_ptr_->device_id);
    if (imp_ptr_->command_queue == nullptr)
    {
        std::cout<<"create command queue error"<<std::endl;
        imp_ptr_->cleanup();
        return false;
    }
    return true;
}

cl_command_queue CLManager::getCommandQueue()
{
    return imp_ptr_->command_queue;
}

cl_context CLManager::getContext()
{
    return imp_ptr_->context;
}

cl_program CLManager::CreatProgam(std::string cl_filename)
{
    auto program = imp_ptr_->CreatProgam(imp_ptr_->context, imp_ptr_->device_id, cl_filename.c_str());
    if (program == nullptr)
    {
        std::cout<<"create program fail"<<std::endl;
        return nullptr;
    }
    imp_ptr_->programs.push_back(program);
    return program;
}

cl_kernel CLManager::CreateKernel(cl_program program, std::string kernel_name)
{
    auto kernel = clCreateKernel(program, kernel_name.c_str(), NULL);
    if (kernel == NULL)
    {
        std::cerr << "Failed to create kernel" << std::endl;
        return nullptr;
    }
    imp_ptr_->kernels.push_back(kernel);
    return kernel;
}

