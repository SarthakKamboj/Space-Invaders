#include "filepath.h"
#include <string.h>
#include <exception>
#include <iostream>
#include <vector>

filepath_t::filepath_t(const char* path) {

	if (!is_valid_path(path)) {
		throw std::exception("path does not exist");
	}

	int str_len = strlen(path);
	path_str = new char[str_len + 1];
	memcpy(path_str, path, str_len);
	path_str[str_len] = 0;

	if (str_len == 0) {
		num_parts = 0;
	}
	else {
		num_parts = 1;
		for (int i = 0; i < str_len; i++) {
			if (path_str[i] == '\\') {
				num_parts += 1;
			}
		}
	}
}

bool filepath_t::is_valid_path(const char* path) {
	DWORD file_attrib = GetFileAttributesA(path);
	return file_attrib != INVALID_FILE_ATTRIBUTES;
}

bool filepath_t::is_valid_path() {
	if (path_str == NULL) return false;
	return is_valid_path(path_str);
}

void filepath_t::go_to_parent_directory() {
	char* last_occur = strrchr(path_str, '\\');
	if (last_occur == NULL) return;
	int new_size = last_occur - path_str + 1;
	char* new_path_str = new char[new_size];
	memcpy(new_path_str, path_str, new_size);
	new_path_str[new_size - 1] = 0;
	delete[] path_str;
	path_str = new_path_str;
	num_parts -= 1;
}

void filepath_t::append_to_path(const char new_part[MAX_PATH]) {
	int path_len = strlen(path_str);
	int new_part_len = strlen(new_part);
	int new_size = path_len + new_part_len + 2;
	char* new_path_str = new char[new_size];
	memcpy(new_path_str, path_str, path_len);
	new_path_str[path_len] = '\\';
	memcpy(new_path_str + path_len + 1, new_part, new_part_len);
	new_path_str[new_size - 1] = 0;

	if (!is_valid_path(new_path_str)) {
		delete[] new_path_str;
		throw std::exception("this new path is not valid");
	}

	delete[] path_str;
	path_str = new_path_str;
}

bool filepath_t::is_directory() {
	if (path_str == NULL) return false;
	DWORD file_attrib = GetFileAttributesA(path_str);
	return (file_attrib & FILE_ATTRIBUTE_DIRECTORY);
}

void filepath_t::list_subdir_and_subfiles(filenames_t& filename_chunks) {

	if (!is_directory()) return;

	int path_len = strlen(path_str);
	if (path_len + 3 > MAX_PATH) {
		throw std::exception("directory path is too long");
	}
	// may need to come back and handle having proper access permissions
	WIN32_FIND_DATAA win32_find_data;

	const char extended_file_name[MAX_PATH] = {};
	memcpy((void*)extended_file_name, path_str, path_len);
	memcpy((void*)(extended_file_name + path_len), "\\*", 2);

	HANDLE cur_file = FindFirstFileA(extended_file_name, &win32_find_data);

	do {
		std::array<char, MAX_PATH> filename;
		memcpy(filename.data(), win32_find_data.cFileName, MAX_PATH);
		filename_chunks.push_back(filename);
	} while (FindNextFileA(cur_file, &win32_find_data) != 0);

	DWORD error = GetLastError();
	if (error != ERROR_NO_MORE_FILES) {
		throw std::exception("cam across an error");
	}

	FindClose(cur_file);
}

const char* filepath_t::get_filename() {
	if (is_directory()) return NULL;
	char* last_occur = strrchr(path_str, '\\');
	return last_occur + 1;
}