#pragma once

#include <vector>

template <typename T, int max_num>
class pool_alloc_t {
public:
	pool_alloc_t();
	~pool_alloc_t();
	T* allocate();
	void deallocate(T* ptr);

private:
	std::vector<T*> free;
	int num_elements_free;
	T* pool_mem;
	int idx_of_last_valid_pool_block;
};

template <typename T, int max_num>
pool_alloc_t<T, max_num>::pool_alloc_t() {
	pool_mem = new T[max_num];
	free.resize(max_num);
	for (int i = 0; i < max_num; i++) {
		free[i] = pool_mem + i;
	}
	idx_of_last_valid_pool_block = max_num - 1;
}

template <typename T, int max_num>
pool_alloc_t<T, max_num>::~pool_alloc_t() {
	delete pool_mem;
}

template <typename T, int max_num>
T* pool_alloc_t<T, max_num>::allocate() {
	if (idx_of_last_valid_pool_block == -1) {
		throw std::exception("cannot allocate anymore");
	}

	T* ptr = free[idx_of_last_valid_pool_block];
	idx_of_last_valid_pool_block -= 1;

	return ptr;
}

template <typename T, int max_num>
void pool_alloc_t<T, max_num>::deallocate(T* ptr) {
	idx_of_last_valid_pool_block += 1;
	free[idx_of_last_valid_pool_block] = ptr;
}
