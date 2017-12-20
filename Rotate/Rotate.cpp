// Rotate.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <CL/cl.h>
#include "CLManager.h"

int _tmain(int argc, _TCHAR* argv[])
{
    CLManager clm;
    clm.Init();
    cl_context context = clm.getContext();
    cl_command_queue command_queue = clm.getCommandQueue();
    cl_program program = clm.CreatProgam("vec_add.cl");
    cl_kernel kernel = clm.CreateKernel(program,"vec_add_kernel");

	return 0;
}

