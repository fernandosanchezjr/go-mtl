// +build darwin

#include "mtl.h"

struct ComputePipelineState {
	void * 			ComputePipelineState;
	uint_t  		MaxTotalThreadsPerThreadgroup;
	uint_t          ThreadExecutionWidth;
	const char * 	Error;
};

struct ComputePipelineState Device_NewComputePipelineStateWithFunction(void * device, void * function);