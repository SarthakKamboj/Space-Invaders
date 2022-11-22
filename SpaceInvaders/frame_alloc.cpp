#include "frame_alloc.h"

void single_frame_alloc_t::handle_sof() {
	frame_stack_alloc.create_stack_frame(stack_frame);
}

char* single_frame_alloc_t::allocate(int num_bytes) {
	return frame_stack_alloc.allocate(num_bytes);
}

void single_frame_alloc_t::handle_eof() {
	frame_stack_alloc.clear();
}

void double_frame_alloc_t::handle_sof() {
	if (cur_alloc == &frame_stack_alloc_a) {
		cur_alloc->create_stack_frame(stack_frame_a);
	}
	else {
		cur_alloc->create_stack_frame(stack_frame_b);
	}
}

char* double_frame_alloc_t::allocate(int num_bytes) {
	return cur_alloc->allocate(num_bytes);
}

void double_frame_alloc_t::handle_eof() {
	if (&frame_stack_alloc_a == cur_alloc) {
		frame_stack_alloc_b.clear();
		cur_alloc = &frame_stack_alloc_b;
	}
	else {
		frame_stack_alloc_a.clear();
		cur_alloc = &frame_stack_alloc_a;
	}
}
