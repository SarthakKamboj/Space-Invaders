#pragma once

#include <vector>
#include <iostream>

#define STACK_SIZE 1024 * sizeof(float)

class stack_alloc_t {

public:

	struct internal_stack_frame_t {
		char* frame_start;
		int num_bytes;
		stack_alloc_t* stack_alloc;

		void deallocate();
	};

	class stack_frame_t {
	public:
		stack_frame_t();
		~stack_frame_t();
		stack_frame_t(stack_frame_t&& other);
		stack_frame_t& operator=(stack_frame_t&& other);

	private:
		internal_stack_frame_t* internal_stack_frame;
		friend class stack_alloc_t;
	};

public:
	stack_alloc_t();
	~stack_alloc_t();
	char* cur;
	char* allocate(int num_bytes);
	void deallocate();
	void clear();
	void create_stack_frame(stack_alloc_t::stack_frame_t& stack_frame);

private:
	char* stack;
	std::vector<internal_stack_frame_t> stack_frames;
};


