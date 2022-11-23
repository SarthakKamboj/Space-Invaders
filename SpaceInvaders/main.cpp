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
		// stack_alloc_t::stack_frame_t frame;
		stack.create_stack_frame();

		int* a_ptr = (int*)stack.allocate(sizeof(int));
		*a_ptr = 5;

		char* b_ptr = stack.allocate(sizeof(char));
		*b_ptr = 0x0a;

		char* c_ptr = stack.allocate(sizeof(char));
		*c_ptr = 0x0b;

		stack.remove_stack_frame();
	}

	{
		// stack_alloc_t::stack_frame_t frame;
		stack.create_stack_frame();

		double* a_ptr = (double*)stack.allocate(sizeof(double));
		*a_ptr = 25;

		char* b_ptr = stack.allocate(sizeof(char));
		*b_ptr = 0x0a;

		stack.remove_stack_frame();
	}


#elif TEST_POOL_ALLOC
	/*
	const int pool_size = 20;
	pool_alloc_t<int, pool_size> int_pool;

	typedef pool_chunk_t<int, pool_size> pct;

	pct* a_ptr = int_pool.allocate();
	*a_ptr->data_ptr = 5;

	pct* b_ptr = int_pool.allocate();
	*b_ptr->data_ptr = 10;

	int_pool.deallocate(b_ptr);
	pct* c_ptr = int_pool.allocate();
	*c_ptr->data_ptr = 20;

	for (int i = 0; i < pool_size - 2; i++) {
		int_pool.allocate();
	}
	*/

	const int pool_size = 1;
	pool_alloc_t<int, pool_size> int_pool;

	typedef pool_chunk_t<int, pool_size> pct;

	pct* a_ptr = int_pool.allocate();
	*a_ptr->data_ptr = 5;
	int_pool.deallocate(a_ptr);

	pct* b_ptr = int_pool.allocate();
	*b_ptr->data_ptr = 10;

	int_pool.deallocate(b_ptr);
	pct* c_ptr = int_pool.allocate();
	*c_ptr->data_ptr = 20;

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