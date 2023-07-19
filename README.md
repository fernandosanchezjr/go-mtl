# 🍏 go-mtl: Go Bindings for Apple Metal
![Build Status](https://github.com/hupe1980/go-mtl/workflows/build/badge.svg) 
[![Go Reference](https://pkg.go.dev/badge/github.com/hupe1980/go-mtl.svg)](https://pkg.go.dev/github.com/hupe1980/go-mtl)
> go-mtl provides seamless integration between Go and Apple Metal, enabling developers to harness the full potential of Metal's high-performance graphics and compute capabilities in their Go applications. With go-mtl, you can write efficient and scalable code for darwin platforms, leveraging Metal's advanced features such as parallel processing, GPU acceleration, and low-level access to the graphics pipeline.

## Installation
Use Go modules to include go-mtl in your project:
```bash
go get github.com/hupe1980/go-mtl
```

## Usage
```go
import (
	"fmt"
	"log"
	"unsafe"

	"github.com/hupe1980/go-mtl"
)

const source = `#include <metal_stdlib>

using namespace metal;

kernel void add_arrays(device const float* inA,
	device const float* inB,
	device float* result,
	uint index [[thread_position_in_grid]])
{
// the for-loop is replaced with a collection of threads, each of which
// calls this function.
result[index] = inA[index] + inB[index];
}
`

func main() {
	// Create a Metal device.
	device, err := mtl.CreateSystemDefaultDevice()
	if err != nil {
		log.Fatal(err)
	}

	// Create a Metal library from the provided source code.
	lib, err := device.NewLibraryWithSource(source)
	if err != nil {
		log.Fatal(err)
	}

	// Retrieve the Metal function named "add_arrays" from the library.
	addArrays, err := lib.NewFunctionWithName("add_arrays")
	if err != nil {
		log.Fatal(err)
	}

	// Create a Metal compute pipeline state with the function.
	pipelineState, err := device.NewComputePipelineStateWithFunction(addArrays)
	if err != nil {
		log.Fatal(err)
	}

	// Create a Metal command queue to submit commands for execution.
	q := device.NewCommandQueue()

	// Set the length of the arrays.
	arrLen := uint(4)

	// Prepare the input data.
	dataA := []float32{0.0, 1.0, 2.0, 3.0}
	dataB := []float32{0.0, 1.0, 2.0, 3.0}

	// Create Metal buffers for input and output data.
	// b1 and b2 represent the input arrays, and r represents the output array.
	b1 := device.NewBufferWithBytes(unsafe.Pointer(&dataA[0]), unsafe.Sizeof(dataA), mtl.ResourceStorageModeShared)
	b2 := device.NewBufferWithBytes(unsafe.Pointer(&dataB[0]), unsafe.Sizeof(dataB), mtl.ResourceStorageModeShared)
	r := device.NewBufferWithLength(unsafe.Sizeof(arrLen), mtl.ResourceStorageModeShared)

	// // Create a Metal command buffer to encode and execute commands.
	cb := q.CommandBuffer()

	// Create a compute command encoder to encode compute commands.
	cce := cb.ComputeCommandEncoder()

	// Set the compute pipeline state to specify the function to be executed.
	cce.SetComputePipelineState(pipelineState)

	// Set the input and output buffers for the compute function.
	cce.SetBuffer(b1, 0, 0)
	cce.SetBuffer(b2, 0, 1)
	cce.SetBuffer(r, 0, 2)

	// Specify threadgroup size
	tgs := pipelineState.MaxTotalThreadsPerThreadgroup
	if tgs > arrLen {
		tgs = arrLen
	}

	// Dispatch compute threads to perform the calculation.
	cce.DispatchThreads(mtl.Size{Width: arrLen, Height: 1, Depth: 1}, mtl.Size{Width: tgs, Height: 1, Depth: 1})

	// End encoding the compute command.
	cce.EndEncoding()

	// Commit the command buffer for execution.
	cb.Commit()

	// Wait until the command buffer execution is completed.
	cb.WaitUntilCompleted()

	// Read the results from the output buffer
	result := (*[1 << 30]float32)(r.Contents())[:arrLen]

	// Print the results.
	fmt.Println(result)
```
Output:
```text
[0 2 4 6]
```

For more example usage, see [examples](./examples).

## Contributing
Contributions are welcome! Feel free to open an issue or submit a pull request for any improvements or new features you would like to see.

## License
This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for details.