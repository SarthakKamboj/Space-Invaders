#pragma once

#include "allocators/pool_alloc.h"
#include <windows.h>
#include <array>
#include <vector>

#define filepath_char char[MAX_PATH]
#define MAX_POOL_SIZE_FILEPATH 256

typedef std::vector < std::array<char, MAX_PATH> > filenames_t;

class filepath_t {
public:
	filepath_t(const char* path);
	char* path_str = nullptr;
	void go_to_parent_directory();
	void append_to_path(const char new_part[MAX_PATH]);
	bool is_directory();
	void list_subdir_and_subfiles(filenames_t& filename_chunks);
	const char* get_filename();
	bool is_valid_path();
	bool is_valid_path(const char* path);

private:
	int num_parts = 0;

};
