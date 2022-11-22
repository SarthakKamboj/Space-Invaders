#include <iostream>
#include "stack_alloc.h"
#include "pool_alloc.h"

#define TEST_STACK_ALLOC 1
#define TEST_POOL_ALLOC 0

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

#endif
}