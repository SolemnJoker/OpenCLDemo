//测试
#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <CL/cl.h>
#include "CLManager.h"


bool CreateMemObject(cl_context context, std::vector<float> a, std::vector<float> b, cl_mem* memObj);
int _tmain(int argc, _TCHAR* argv[])
{
    const int ARRAY_SIZE = 1000;
    CLManager clm;
    clm.Init();
    cl_context context = clm.getContext();
    cl_command_queue command_queue = clm.getCommandQueue();
    cl_program program = clm.CreatProgam("vec_add.cl");
    cl_kernel kernel = clm.CreateKernel(program,"vec_add_kernel");
    cl_mem memObj[3] = {0};

    auto cleanup = [&](){
        for (int i = 0; i < 3; i++)
        {
            if (memObj[i] != 0)
                clReleaseMemObject(memObj[i]);
        }
    };

    std::vector<float> a(ARRAY_SIZE);
    std::vector<float> b(ARRAY_SIZE);
    std::vector<float> result(ARRAY_SIZE);
    for (auto i = 0;i<ARRAY_SIZE;i++)
    {
        a[i] = i;
        b[i] = i*2;
        result[i] = 0;
    }
    if (!CreateMemObject(context,a,b,memObj))
    {
        cleanup();
    }
    int err = 0;
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), memObj);
    err|= clSetKernelArg(kernel, 1, sizeof(cl_mem), memObj + 1);
    err|= clSetKernelArg(kernel, 2, sizeof(cl_mem), memObj + 2);

    if (err != CL_SUCCESS)
    {
        std::cerr << "Error setting kernel arguments." << std::endl;
        cleanup();
        return 1;
    }

    size_t globalWorkSize[1] = { ARRAY_SIZE };
    size_t localWorkSize[1] = { 1};

    // Queue the kernel up for execution across the array
    err = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
                                    globalWorkSize, localWorkSize,
                                    0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        std::cerr << "Error queuing kernel for execution." << std::endl;
        cleanup();
        return 1;
    }

    // Read the output buffer back to the Host
    err = clEnqueueReadBuffer(command_queue, memObj[2], CL_TRUE,
                                 0, ARRAY_SIZE * sizeof(float), &result[0],
                                 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        std::cerr << "Error reading result buffer." << std::endl;
        cleanup();
        return 1;
    }

    // Output the result buffer
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        std::cout << result[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "Executed program succesfully." << std::endl;
    cleanup();

	return 0;
}



bool CreateMemObject(cl_context context, std::vector<float> a, std::vector<float> b, cl_mem* memObj)
{
    memObj[0] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, a.size()*sizeof(float), &a[0], nullptr);
    memObj[1] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, b.size()*sizeof(float), &b[0], nullptr);
    memObj[2] = clCreateBuffer(context, CL_MEM_WRITE_ONLY , b.size()*sizeof(float), nullptr, nullptr);
    if (memObj[0] == nullptr||memObj[1] == nullptr||memObj[1] == nullptr)
    {
        std::cout<<"create mem error"<<std::endl;
        return false;
    }

}