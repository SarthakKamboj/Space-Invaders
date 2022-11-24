#pragma once

#include "windows.h"
#include "allocators/pool_alloc.h"

namespace file_io {

	void read_async_completion_routine(
		DWORD dwErrorCode,
		DWORD dwNumberOfBytesTransfered,
		LPOVERLAPPED lpOverlapped
	);

	int get_file_size(const char* file_path);
	void read_file_sync(const char* file_path, char* output_buffer, int amount_bytes_to_read, int& amount_bytes_read);
	typedef void(*read_async_callback)(int);
	void read_file_async(const char* file_path, char* output_buffer, int amount_bytes_to_read, read_async_callback callback);
	bool delete_file_sync(const char* file_path);
	bool create_file(const char* file_path, bool overwrite);
	bool create_dir(const char* dir_path);
	bool is_file(const char* file_path);
	bool exists(const char* file_path);
}
