/**************************************************************************
Copyright	: Cloudream Inc All Rights Reserved
Author		: xuwj
Date        : 2017/12/20 15:59
Description	: 
**************************************************************************/
#ifndef CLMANAGER_H_
#define CLMANAGER_H_
#include <memory>
#include <CL/cl.h>
#include <string>
class CLManager
{
    class Imp;
    std::shared_ptr<Imp> imp_ptr_;
public:
    CLManager();
    ~CLManager();

    bool Init();
    cl_command_queue getCommandQueue();
    cl_context getContext();
    cl_program CreatProgam(std::string cl_filename);
    cl_kernel CreateKernel(cl_program program,std::string kernel_name);
private:
};

#endif //CLMANAGER_H_
