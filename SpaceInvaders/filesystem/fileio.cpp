#include "fileio.h"
#include <exception>

int file_io::get_file_size() {
	return -1;
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

bool file_io::create_file(const char* file_path, bool overwrite) {
	DWORD file_creation_policy = CREATE_NEW;
	if (overwrite) {
		file_creation_policy = CREATE_ALWAYS;
	}

	HANDLE file_handle = CreateFileA(file_path, GENERIC_READ | GENERIC_WRITE, 0, NULL, file_creation_policy, FILE_ATTRIBUTE_NORMAL, NULL);

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

bool file_io::delete_file(const char* file_path) {
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
