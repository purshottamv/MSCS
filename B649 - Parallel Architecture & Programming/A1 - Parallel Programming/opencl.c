#include <stdio.h> 
#include <stdlib.h> 
#include "CL/cl.h" 

#define ROWS 1500
#define COLUMNS 1500
#define ELEMENT_VALUE 1


// The Kernel Program
const char *kernel_source_code = "__kernel void matrixMultiply(__global int *mO,__global int *m1,__global int *m2,uint matDimension)\n"\
								"{\n"\
								"int global_ID_x = get_global_id(0);\n"\
								"int global_ID_y = get_global_id(1);\n"\
								"int resultMatElement = 0;\n"\
								"for (int i=0; i< matDimension; i++)\n"\
								"{\n"\
								"int m1x = m1[global_ID_y * matDimension + i];\n"\
								"int m2y = m2[i * matDimension + global_ID_x];\n"\
								"resultMatElement += m1x * m2y;\n"\
								"}\n"\
								"mO[global_ID_y * matDimension + global_ID_x] = resultMatElement;\n"\
								"}\n";

int *mat1;
int *mat2;
int *res_mat;


// Initialize mat1 and mat2
void fillmatrix()
{
	int i;
	int j;
	mat1 = (int*)malloc(sizeof(int)*ROWS*COLUMNS);
	mat2 = (int*)malloc(sizeof(int)*ROWS*COLUMNS);
	res_mat = (int*)malloc(sizeof(int)*ROWS*COLUMNS);
	for(i=0; i<ROWS; i++)
	{
		for(j=0; j<COLUMNS; j++)
		{
			mat1[i*COLUMNS+j]= ELEMENT_VALUE;
			mat2[i*COLUMNS+j]= ELEMENT_VALUE;
			res_mat[i*COLUMNS+j]= 0;
		}
	}
}

// Display Final Matrix
void outputFinalMatrix()
{
	cl_uint height = ROWS;
	int i;
	int j;
	for(i=0; i<ROWS; i++)
	{
		for(j=0; j<COLUMNS; j++)
		{
			printf("%d ",res_mat[i*height+j]);
		}
		printf("\n");
	}
}

	
int main()
{ 
	long seconds, useconds, mtime;
	struct timeval start, end;
	
	cl_uint nRows = ROWS;
	
	// OpenCL host variables
	cl_uint num_devices;
	cl_context_properties properties[3];
	cl_device_id device_id;
	cl_int err, work_dim;
	cl_platform_id platform_id;
	cl_uint num_platforms_available;
	cl_context context;
	cl_command_queue command_queue;
	cl_program program;
	cl_kernel kernel;
	cl_mem input_buffer_mat1,input_buffer_mat2, output_buffer;
	size_t global[2];
	
	fillmatrix();

	// Step 1 : Obtain the list of Platforms available
	err = clGetPlatformIDs(5,&platform_id,&num_platforms_available);
	
	if (err != CL_SUCCESS)
	{
		printf("Unable to get Platform ID. Error Code=%d\n",err);
		exit(1);
 
	}
	else
	{
		printf("Number of OpenCL platforms Available = %d\n",num_platforms_available);
	}
 
	// Step 2: Obtain the list of devices available on a platform
	//err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &num_devices);
	err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 5, &device_id, &num_devices);
 
	if (err != CL_SUCCESS)
 	{
 		printf("Unable to get Device ID. Error Code=%d\n",err);
 		exit(1);
 	}
	else
	{
		printf("Number of OpenCL devices available : %d\n",num_devices);
	}

	// Step 3 : Context properties list - must be terminated with 0
 	properties[0]= CL_CONTEXT_PLATFORM;
 	properties[1]= (cl_context_properties) platform_id;
 	properties[2]= 0;
 
	// Step 4 : create the OpenCL context
 	context = clCreateContext(properties, 1, &device_id, NULL, NULL, &err);
	if (err != CL_SUCCESS)
	{
		printf("Unable to create context. Error Code=%d\n",err);
		exit(1);
	}

	// Step 5 : Create a command queue on a specific device
	// The OpenCL functions that are submitted to a command-queue are enqueued in the order the
	// calls are made but can be configured to execute in-order or out-of-order. 
	// The properties argument in clCreateCommandQueue can be used to specify the execution order.
	command_queue = clCreateCommandQueue(context,device_id, 0, &err);
	if (err != CL_SUCCESS)
	{
		printf("Unable to create command queue. Error Code=%d\n",err);
 		exit(1);
	}
 
	// Step 6 : Create program object from source. kernel_source_code contains
	// Creates a program object for a context, and loads the source code 
	// specified by the text strings in kernel_source_code into the program object. 
	program = clCreateProgramWithSource(context, 1 ,(const char **) &kernel_source_code, NULL, &err);
 
	if (err != CL_SUCCESS)
	{
		printf("Unable to create program object. Error Code=%d\n",err);
		exit(1);
	}
	// Step 7 : Build (Compilation and linking) the program executable from the program source. 
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		printf("Build failed. Error Code=%d\n", err);
		size_t len;
		char buffer[4096];
		// get the build log
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, NULL);
		printf("--- Build Log -- \n %s\n",buffer);
		exit(1);
	}

	// Step 8 : Create the kernel Object. Provide the function name with the __kernel qualifier
	kernel = clCreateKernel(program, "matrixMultiply", &err);
	if (err != CL_SUCCESS)
	{
		printf("Unable to create kernel object. Error Code=%d\n",err);
		exit(1);
	}

	// Step 9 : Create the buffer objects to input and output args of kernel function
	input_buffer_mat1 = clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, sizeof(int) * ROWS*COLUMNS, mat1, NULL);
	input_buffer_mat2 = clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, sizeof(int) * ROWS*COLUMNS, mat2, NULL);
	output_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * ROWS*COLUMNS, NULL, NULL);

	// Step 10 : Set the argument list for the kernel command
	if ( clSetKernelArg(kernel, 0, sizeof(cl_mem), &output_buffer) ||
		clSetKernelArg(kernel, 1, sizeof(cl_mem), &input_buffer_mat1) ||
		clSetKernelArg(kernel, 2, sizeof(cl_mem), &input_buffer_mat2) ||
		clSetKernelArg(kernel, 3, sizeof(cl_uint), &nRows) != CL_SUCCESS)
		{
			printf("Unable to set kernel arguments. Error Code=%d\n",err);
			exit(1);
		}
	
	// set the global & local work size
//	global = nRows;
	global[0]= nRows;
	global[1]= nRows;

	gettimeofday(&start, NULL);

	// Step 11 : Enqueue the command to execute a kernel on the device
	work_dim = 2;		// The number of dimensions used to specify the global work-items 
						//and work-items in the work-group. work_dim > 0 || work_dim >= 3. 
	err = clEnqueueNDRangeKernel(command_queue, // The kernel will be queued for execution on the device associated with command_queue.
								kernel,	//	Kernel Object
								work_dim, // The number of dimensions used to specify the global work-items and work-items in the work-group
								NULL, // Global work offset - for future - Now it is NULL
								global, //	Global work size - Number of global work-items in work_dim dimensions that will execute the kernel function
								NULL, // Local Work Size - Number of work items that make up a work group
								0,	// Number of events in wait list
								NULL, // Event wait list - Events that need to complete before this particular command can be executed.
								NULL); // Returns the event object that identifies this particular kernel execution instance.
	if (err != CL_SUCCESS)
	{
		printf("Unable to enqueue kernel command. Error Code=%d\n",err);
		exit(1);
	}
	
	// Step 12 : wait for the command to finish
	// Blocks until all previously queued OpenCL commands in a 
	// command-queue are issued to the associated device and have completed. 
	clFinish(command_queue);		// clFlush is the non-blocking equivalent

	// Step 13 : read the output back to host memory
	err = clEnqueueReadBuffer(command_queue, output_buffer, CL_FALSE, 0, sizeof(int) * ROWS*COLUMNS, res_mat, 0, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		printf("Error enqueuing read buffer command. Error Code=%d\n",err);
		exit(1);
	}
	
	// clean up
	clReleaseMemObject(input_buffer_mat1);
	clReleaseMemObject(input_buffer_mat2);
	clReleaseMemObject(output_buffer);
	clReleaseProgram(program);
	clReleaseKernel(kernel);

	gettimeofday(&end, NULL);
	seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;
	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

	printf("\nParallel Program using OpenCL :");
	printf(" %ld milliseconds\n\n", mtime);

//	outputFinalMatrix();
	return 0;
}