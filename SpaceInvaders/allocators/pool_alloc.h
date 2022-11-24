#pragma once

#include <vector>
#include <array>

// Must be pool return pointer to actual object, not pool_chunk_t

template <typename T, int max_num>
class pool_alloc_t;

template <typename T, int max_num>
class pool_chunk_t {
public:
	T* data_ptr = NULL;
private:
	pool_chunk_t* next = NULL;

	friend class pool_alloc_t<T, max_num>;
};

template <typename T, int max_num>
class pool_alloc_t {
public:
	pool_alloc_t();
	~pool_alloc_t();
	// pool_chunk_t<T, max_num>* allocate();
	T* allocate();
	// void deallocate(pool_chunk_t<T, max_num>* chunk_ptr);
	void deallocate(T* data_ptr);

private:
	int num_elements_free = 0;
	T* pool_mem = NULL;
	pool_chunk_t<T, max_num>* pool_chunk_mem_free = NULL;
	pool_chunk_t<T, max_num>* pool_chunk_mem_free_head = NULL;
	pool_chunk_t<T, max_num>* pool_chunk_mem_used_head = NULL;
};

template <typename T, int max_num>
pool_alloc_t<T, max_num>::pool_alloc_t() {
	pool_mem = new T[max_num];
	pool_chunk_mem_free = new pool_chunk_t<T, max_num>[max_num];

	pool_chunk_mem_free[0].data_ptr = &pool_mem[0];
	pool_chunk_mem_free[0].next = NULL;
	pool_chunk_mem_free_head = &pool_chunk_mem_free[0];

	for (int i = 1; i < max_num; i++) {
		pool_chunk_mem_free[i].data_ptr = &pool_mem[i];
		pool_chunk_mem_free[i - 1].next = &pool_chunk_mem_free[i];
	}
	num_elements_free = max_num;
}

template <typename T, int max_num>
pool_alloc_t<T, max_num>::~pool_alloc_t() {
	delete pool_mem;
	delete pool_chunk_mem_free;
}

/*
template <typename T, int max_num>
pool_chunk_t<T, max_num>* pool_alloc_t<T, max_num>::allocate() {
	if (num_elements_free == 0) {
		throw std::exception("cannot allocate anymore");
	}

	pool_chunk_t<T, max_num>* old_head = &pool_chunk_mem_free_head[0];
	pool_chunk_t<T, max_num>* new_head = pool_chunk_mem_free_head[0].next;
	pool_chunk_mem_free_head = new_head;
	old_head->next = NULL;
	num_elements_free -= 1;

	// *old_head->data_ptr = {};

	return old_head;
}
*/

template <typename T, int max_num>
T* pool_alloc_t<T, max_num>::allocate() {
	if (num_elements_free == 0) {
		throw std::exception("cannot allocate anymore");
	}

	pool_chunk_t<T, max_num>* old_head = &pool_chunk_mem_free_head[0];
	pool_chunk_t<T, max_num>* new_head = pool_chunk_mem_free_head[0].next;
	pool_chunk_mem_free_head = new_head;
	num_elements_free -= 1;

	// *old_head->data_ptr = {};
	old_head->next = NULL;
	T* data_ptr = old_head->data_ptr;
	old_head->data_ptr = NULL;
	if (pool_chunk_mem_used_head == NULL) {
		pool_chunk_mem_used_head = old_head;
	}
	else {
		old_head->next = pool_chunk_mem_used_head;
		pool_chunk_mem_used_head = old_head;
	}

	// return old_head;
	return data_ptr;
}

/*
template <typename T, int max_num>
void pool_alloc_t<T, max_num>::deallocate(pool_chunk_t<T, max_num>* chunk_ptr) {
	if (num_elements_free == 0) {
		pool_chunk_mem_free_head = chunk_ptr;
		chunk_ptr->next = NULL;
	}
	else {
		pool_chunk_t<T, max_num>* old_head = pool_chunk_mem_free_head;
		pool_chunk_mem_free_head = chunk_ptr;
		chunk_ptr->next = old_head;
	}

	num_elements_free += 1;

}
*/


template <typename T, int max_num>
void pool_alloc_t<T, max_num>::deallocate(T* data_ptr) {

	pool_chunk_t<T, max_num>* chunk_ptr = pool_chunk_mem_used_head;
	pool_chunk_mem_used_head = pool_chunk_mem_used_head->next;
	chunk_ptr->data_ptr = data_ptr;

	if (num_elements_free == 0) {
		pool_chunk_mem_free_head = chunk_ptr;
		chunk_ptr->next = NULL;
	}
	else {
		pool_chunk_t<T, max_num>* old_head = pool_chunk_mem_free_head;
		pool_chunk_mem_free_head = chunk_ptr;
		chunk_ptr->next = old_head;
	}

	num_elements_free += 1;
}



