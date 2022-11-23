#include "stack_alloc.h"
#include <exception>

stack_alloc_t::stack_alloc_t() {
	stack = new char[STACK_SIZE];
	cur = stack;
}

stack_alloc_t::~stack_alloc_t() {
	cur = nullptr;
	delete stack;
}

char* stack_alloc_t::allocate(int num_bytes) {
	if (num_stack_frames == 0) {
		throw std::exception("must declare at least 1 stack frame");
	}

	int cur_bytes_allocated = cur - stack;
	int new_bytes_alloc = cur_bytes_allocated + num_bytes + sizeof(int);
	if (new_bytes_alloc > STACK_SIZE) {
		throw std::exception("stack overflow...cannot allocate");
	}

	char* cur_saved = cur;
	cur += num_bytes;
	num_bytes_of_cur_stack_frame += num_bytes;
	memset(cur_saved, 0, num_bytes);
	return cur_saved;
}

void stack_alloc_t::create_stack_frame() {
	if (num_stack_frames > 0) {
		*cur = num_bytes_of_cur_stack_frame;
		cur += sizeof(num_bytes_of_cur_stack_frame);
	}
	num_stack_frames += 1;
	num_bytes_of_cur_stack_frame = 0;
}

void stack_alloc_t::remove_stack_frame() {
	cur -= num_bytes_of_cur_stack_frame;
	num_stack_frames -= 1;
	if (num_stack_frames > 0) {
		num_bytes_of_cur_stack_frame = *cur;
		cur -= sizeof(num_bytes_of_cur_stack_frame);
	}
}
void stack_alloc_t::clear() {
	cur = stack;
	num_stack_frames = 0;
	num_bytes_of_cur_stack_frame = 0;
}
