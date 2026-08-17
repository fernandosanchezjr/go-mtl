// +build darwin

#include "mtl.h"

void * Device_NewCommandQueue(void * device);

void * CommandQueue_CommandBuffer(void * commandQueue);
void CommandQueue_Release(void * commandQueue);