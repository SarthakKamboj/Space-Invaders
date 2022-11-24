#include "fileio.h"
#include <exception>
#include <iostream>
#include <unordered_map>

// OVERLAPPED overlap_for_read_async = {};
extern int read_async;
std::unordered_map<HANDLE, file_io::read_async_callback> callback_map;

int file_io::get_file_size(const char* file_path) {
	HANDLE handle = CreateFileA(file_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE) {
		throw std::exception("could not open file");
	}
	LARGE_INTEGER size;
	bool success = GetFileSizeEx(handle, &size);
	if (!success) {
		throw std::exception("ran into error trying to get file size");
	}
	return (int)size.QuadPart;
}

void file_io::read_file_sync(const char* file_path, char* output_buffer, int amount_bytes_to_read, int& amount_bytes_read) {
	if (file_path == NULL) {
		throw std::exception("file path is null");
	}

	if (!is_file(file_path)) {
		throw std::exception("file path is a directory");
	}

	DWORD file_attrib = GetFileAttributesA(file_path);
	if (file_attrib & FILE_ATTRIBUTE_READONLY) {
		throw std::exception("this file is read only");
	}

	HANDLE handle = CreateFileA(file_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE) {
		throw std::exception("could not open file");
	}

	DWORD num_bytes_read;
	bool success = ReadFile(handle, output_buffer, amount_bytes_to_read, &num_bytes_read, NULL);
	if (!success) {
		CloseHandle(handle);
		throw std::exception("could not read file");
	}

	amount_bytes_read = num_bytes_read;

	CloseHandle(handle);
	return;
}

void file_io::read_file_async(const char* file_path, char* output_buffer, int amount_bytes_to_read, read_async_callback callback) {
	if (file_path == NULL) {
		throw std::exception("file path is null");
	}

	if (!is_file(file_path)) {
		throw std::exception("file path is a directory");
	}

	DWORD file_attrib = GetFileAttributesA(file_path);
	if (file_attrib & FILE_ATTRIBUTE_READONLY) {
		throw std::exception("this file is read only");
	}

	// HANDLE handle = CreateFileA(file_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	HANDLE handle = CreateFileA(file_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	HANDLE hReadEvent = CreateEventA(0, TRUE, FALSE, "ReadCompletionEvent");
	std::cout << "handle: " << handle << std::endl;
	std::cout << "hReadEvent: " << hReadEvent << std::endl;
	LPOVERLAPPED overlap_for_read_async = new OVERLAPPED;
	memset(overlap_for_read_async, 0, sizeof(OVERLAPPED));
	overlap_for_read_async->hEvent = hReadEvent;
	callback_map[hReadEvent] = callback;
	// overlap_for_read_async->Pointer = callback;
	int success = ReadFileEx(handle, output_buffer, amount_bytes_to_read, overlap_for_read_async, (LPOVERLAPPED_COMPLETION_ROUTINE)file_io::read_async_completion_routine);
	DWORD error = GetLastError();
	if (!success || error != ERROR_SUCCESS) {
		throw std::exception("cannot put read file async request on queue");
	}
	// WaitForSingleObjectEx(hReadEvent, INFINITE, true);
	WaitForSingleObjectEx(hReadEvent, INFINITE, true);
	std::cout << "done in read_file_async" << std::endl;
}


void file_io::read_async_completion_routine(DWORD dwErrorCode,
	DWORD dwNumberOfBytesTransfered,
	LPOVERLAPPED lpOverlapped) {
	if (dwErrorCode != ERROR_SUCCESS) {
		throw std::exception("could not read file asynchronously");
	}
	std::cout << "successfully done reading async" << std::endl;
	read_async = 1;
	read_async_callback callback = (read_async_callback)callback_map[lpOverlapped->hEvent];
	callback((int)dwNumberOfBytesTransfered);
	delete lpOverlapped;
}

bool file_io::create_dir(const char* dir_path) {
	bool success = CreateDirectoryA(dir_path, NULL);
	if (!success) {
		DWORD error = GetLastError();
		if (error == ERROR_ALREADY_EXISTS) {
			// directory already exists
			return true;
		}
		throw std::exception("could not make directory");
	}
	return true;
}

bool file_io::create_file(const char* file_path, bool overwrite) {
	DWORD file_creation_policy = CREATE_NEW;
	if (overwrite) {
		file_creation_policy = CREATE_ALWAYS;
	}

	HANDLE file_handle = CreateFileA(file_path, GENERIC_READ | GENERIC_WRITE, 0, NULL, file_creation_policy, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

	if (file_handle == INVALID_HANDLE_VALUE) {
		DWORD error = GetLastError();
		if (!(error & ERROR_FILE_EXISTS)) {
			throw std::exception("could not create file");
		}
		else {
			// file exists but overwrite is not enabled, but we still return true because file exists
			return true;
		}

	}

	bool closed = CloseHandle(file_handle);

	if (!closed) {
		throw std::exception("could not close file handle of newly created file");
	}

	return true;
}

bool file_io::delete_file_sync(const char* file_path) {
	if (!exists(file_path)) {
		throw std::exception("file does not exist");
	}
	if (!is_file(file_path)) {
		throw std::exception("path is for a directory");
	}
	bool successfully_deleted = DeleteFileA(file_path);
	if (!successfully_deleted) {
		DWORD error = GetLastError();
		if (error == ERROR_DS_NO_SUCH_OBJECT) {
			throw std::exception("file does not exist");
		}
	}
	return true;
}

bool file_io::is_file(const char* file_path) {
	if (file_path == NULL) return false;
	DWORD file_attrib = GetFileAttributesA(file_path);
	return !(file_attrib & FILE_ATTRIBUTE_DIRECTORY);
}

bool file_io::exists(const char* file_path) {
	if (file_path == NULL) return false;
	DWORD file_attrib = GetFileAttributesA(file_path);
	return file_attrib != INVALID_FILE_ATTRIBUTES;
}
