#include <stdio.h>
#include "CUDAHandler.h"

bool
CUDAHandler::CheckError(const std::string& message)
{
  if (cudaStatus != 0)
  {
    printf("\nGPU ERROR [%s]: %s\n", message.c_str(), cudaGetErrorString(cudaStatus));
    return true;
  }
  return false;
}

void
CUDAHandler::ActivateGPU()
{
  cudaStatus = cudaSetDevice(gpu_id);
  this->CheckError("Select GPU " + std::to_string(gpu_id));
}

CUDAHandler::CUDAHandler(int gpu_id)
{
  this->gpu_id = gpu_id;
}

CUDAHandler::~CUDAHandler()
{
}

void
CUDAHandler::resetGPU()
{
  cudaStatus = cudaDeviceReset();
  if (cudaStatus != 0)
  {
    printf("\nGPU ERROR: DeInitializeCUDA\n");
  }
}

void
CUDAHandler::InitializeCUDA()
{
  this->resetGPU();
}

void
CUDAHandler::DeInitializeCUDA()
{
  this->resetGPU();
}
