#pragma once

#include "stack_alloc.h"

class single_frame_alloc_t {
public:
	void handle_sof();
	char* allocate(int num_bytes);
	void handle_eof();

private:
	stack_alloc_t frame_stack_alloc;
	stack_alloc_t::stack_frame_t stack_frame;
};

class double_frame_alloc_t {
public:
	char* allocate(int num_bytes);
	void handle_sof();
	void handle_eof();

private:
	stack_alloc_t frame_stack_alloc_a, frame_stack_alloc_b;
	stack_alloc_t::stack_frame_t stack_frame_a, stack_frame_b;
	stack_alloc_t* cur_alloc = &frame_stack_alloc_a;
};
