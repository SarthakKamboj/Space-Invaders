#include <iostream>
#include "stack_alloc.h"
#include "pool_alloc.h"
#include "frame_alloc.h"

#define TEST_STACK_ALLOC 0
#define TEST_POOL_ALLOC 0
#define TEST_SINGLE_FRAME_STACK_ALLOC 0
#define TEST_DOUBLE_FRAME_STACK_ALLOC 1

int main() {

#if TEST_STACK_ALLOC

	stack_alloc_t stack;
	{
		stack_alloc_t::stack_frame_t frame;
		stack.create_stack_frame(frame);

		int* a_ptr = (int*)stack.allocate(sizeof(int));
		*a_ptr = 5;

		char* b_ptr = stack.allocate(sizeof(char));
		*b_ptr = 0x0a;

		char* c_ptr = stack.allocate(sizeof(char));
		*c_ptr = 0x0b;
	}

	{
		stack_alloc_t::stack_frame_t frame;
		stack.create_stack_frame(frame);

		double* a_ptr = (double*)stack.allocate(sizeof(double));
		*a_ptr = 25;

		char* b_ptr = stack.allocate(sizeof(char));
		*b_ptr = 0x0a;
	}


#elif TEST_POOL_ALLOC
	const int pool_size = 20;
	pool_alloc_t<int, pool_size> int_pool;
	int* a_ptr = int_pool.allocate();
	*a_ptr = 5;

	int* b_ptr = int_pool.allocate();
	*b_ptr = 10;

	int_pool.deallocate(b_ptr);
	int* c_ptr = int_pool.allocate();

	*c_ptr = 20;

	for (int i = 0; i < pool_size; i++) {
		int_pool.allocate();
	}

#elif TEST_SINGLE_FRAME_STACK_ALLOC == 1
	single_frame_alloc_t single_frame_alloc;
	for (int i = 0; i < 2; i++) {
		single_frame_alloc.handle_sof();
		int* a_ptr = (int*)single_frame_alloc.allocate(sizeof(int));
		*a_ptr = 5 * i;
		char* b_ptr = (char*)single_frame_alloc.allocate(sizeof(char));
		*b_ptr = 15 * i;
		double* c_ptr = (double*)single_frame_alloc.allocate(sizeof(double));
		*c_ptr = 25 * i;
		single_frame_alloc.handle_eof();
	}

#elif TEST_DOUBLE_FRAME_STACK_ALLOC == 1
	double_frame_alloc_t double_frame_alloc;
	for (int i = 0; i < 4; i++) {
		double_frame_alloc.handle_sof();
		int* a_ptr = (int*)double_frame_alloc.allocate(sizeof(int));
		*a_ptr = 5 * (i + 1);
		char* b_ptr = (char*)double_frame_alloc.allocate(sizeof(char));
		*b_ptr = 15 * (i + 1);
		double* c_ptr = (double*)double_frame_alloc.allocate(sizeof(double));
		*c_ptr = 25 * (i + 1);
		double_frame_alloc.handle_eof();
	}

#endif
}