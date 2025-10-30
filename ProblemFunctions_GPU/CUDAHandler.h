#pragma once
#include <string>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

class CUDAHandler
{
	int gpu_id;
public:
	cudaError_t cudaStatus;
	bool CheckError(const std::string& message);
	void ActivateGPU();
	void resetGPU();
	void InitializeCUDA();
	void DeInitializeCUDA();
	explicit CUDAHandler(int gpu_id);
	~CUDAHandler();
};
