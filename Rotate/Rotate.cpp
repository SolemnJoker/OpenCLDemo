// Rotate.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <CL/cl.h>
#include "CLManager.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

int _tmain(int argc, _TCHAR* argv[])
{
    CLManager clm;
    clm.Init();
    cl_context context = clm.getContext();
    cl_command_queue command_queue = clm.getCommandQueue();
    cl_program program = clm.CreatProgam("rotate.cl");
    cl_kernel kernel = clm.CreateKernel(program,"rotate_kernel");
    cv::Mat img = cv::imread("test.jpg");
    cv::Mat img_result(img.size(),img.type());
    cl_mem raw = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, img.dataend - img.datastart, img.data, nullptr);
    cl_mem result = clCreateBuffer(context, CL_MEM_READ_WRITE , img.dataend - img.datastart, nullptr, nullptr);
    if (raw && result)
    {
        float theta = M_PI*20.0/180.0;
        float sintheta = sin(theta);
        float costheta = cos(theta);
        int err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &raw);
        err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &result);
        err |= clSetKernelArg(kernel, 2, sizeof(int),  (void *)&img.cols);
        err |= clSetKernelArg(kernel, 3, sizeof(int),  (void *)&img.rows);
        err |= clSetKernelArg(kernel, 4, sizeof(float), (void *)&sintheta);
        err |= clSetKernelArg(kernel, 5, sizeof(float), (void *)&costheta);
        if (err == CL_SUCCESS)
        {
            size_t globalThreads[] = { img.cols, img.rows, 3 };
            size_t localThreads[] = { 16, 16 };
            cl_event ev;
            err = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, globalThreads, localThreads, 0, NULL, &ev);
            auto status = clFlush(command_queue);
            clWaitForEvents(1, &ev);

            if (err != CL_SUCCESS)
            {
                std::cerr << "Error queuing kernel for execution." << std::endl;
                return 1;
            }

            // Read the output buffer back to the Host
            err = clEnqueueReadBuffer(command_queue, result, CL_TRUE,
                0, img.dataend - img.datastart, img_result.data,
                0, NULL, NULL);
            int a = 0;
        }
    }
    else
    {
        return 1;
    }

	return 0;
}

