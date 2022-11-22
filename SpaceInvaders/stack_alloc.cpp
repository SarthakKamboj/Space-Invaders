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

	if (stack_frames.size() == 0) {
		throw std::exception("must declare at least 1 stack frame");
	}

	int cur_bytes_allocated = cur - stack;
	int new_bytes_alloc = cur_bytes_allocated + num_bytes + sizeof(int);
	if (new_bytes_alloc > STACK_SIZE) {
		throw std::exception("stack overflow...cannot allocate");
	}
	char* cur_saved = cur;
	cur += num_bytes;
	*cur = num_bytes;
	cur += sizeof(int);

	stack_frames[stack_frames.size() - 1].num_bytes += num_bytes + sizeof(int);

	return cur_saved;
}

void stack_alloc_t::deallocate() {
	if (cur == stack) return;

	if (stack_frames.size() == 0) {
		throw std::exception("must declare at least 1 stack frame");
	}

	cur -= sizeof(int);
	char num_bytes = *cur;
	cur -= num_bytes;
	stack_frames[stack_frames.size() - 1].num_bytes -= num_bytes + sizeof(int);
}

void stack_alloc_t::clear() {
	cur = stack;
	stack_frames.clear();
}

void stack_alloc_t::create_stack_frame(stack_alloc_t::stack_frame_t& user_stack_frame) {
	stack_alloc_t::internal_stack_frame_t frame;
	frame.frame_start = cur;
	frame.stack_alloc = this;
	frame.num_bytes = 0;
	stack_frames.emplace_back(frame);

	user_stack_frame.internal_stack_frame = &stack_frames[stack_frames.size() - 1];
}

stack_alloc_t::stack_frame_t& stack_alloc_t::stack_frame_t::operator=(stack_alloc_t::stack_frame_t&& other) {
	if (internal_stack_frame != NULL) {
		internal_stack_frame->deallocate();
	}

	internal_stack_frame = other.internal_stack_frame;
	other.internal_stack_frame = NULL;

	return *this;
}

stack_alloc_t::stack_frame_t::stack_frame_t() {
	internal_stack_frame = NULL;
	std::cout << "frame constructed" << std::endl;
}

stack_alloc_t::stack_frame_t::~stack_frame_t() {
	if (internal_stack_frame != NULL) {
		internal_stack_frame->deallocate();
		std::cout << "frame destructed" << std::endl;
	}
}

stack_alloc_t::stack_frame_t::stack_frame_t(stack_alloc_t::stack_frame_t&& other) {
	internal_stack_frame = other.internal_stack_frame;
	other.internal_stack_frame = NULL;
}

void stack_alloc_t::internal_stack_frame_t::deallocate() {
	stack_alloc->cur = frame_start;
	std::cout << "upon destruction, this stack frame had " << num_bytes << " bytes allocated" << std::endl;
	stack_alloc->stack_frames.pop_back();
}
