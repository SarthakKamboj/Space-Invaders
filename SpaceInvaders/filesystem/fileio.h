#pragma once

#include "windows.h"

namespace file_io {
	int get_file_size();
	void read_file_sync(const char* file_path, char* output_buffer, int amount_bytes_to_read, int& amount_bytes_read);
	bool delete_file(const char* file_path);
	bool create_file(const char* file_path, bool overwrite);
	bool is_file(const char* file_path);
	bool exists(const char* file_path);
}
