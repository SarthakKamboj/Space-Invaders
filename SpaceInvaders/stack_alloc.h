#pragma once

#include <vector>
#include <iostream>

#define STACK_SIZE 1024 * sizeof(float)

class stack_alloc_t {
public:
	stack_alloc_t();
	~stack_alloc_t();
	char* cur;
	char* allocate(int num_bytes);
	void clear();
	void create_stack_frame();
	void remove_stack_frame();

private:
	char* stack;
	int num_stack_frames = 0;
	int num_bytes_of_cur_stack_frame = 0;
};


