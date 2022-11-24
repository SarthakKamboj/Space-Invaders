#include <iostream>
#include "allocators/stack_alloc.h"
#include "allocators/pool_alloc.h"
#include "allocators/frame_alloc.h"
#include "filesystem/filepath.h"
#include "filesystem/fileio.h"

#define TEST_STACK_ALLOC 0
#define TEST_POOL_ALLOC 0
#define TEST_SINGLE_FRAME_STACK_ALLOC 0
#define TEST_DOUBLE_FRAME_STACK_ALLOC 0
#define TEST_FILEPATH 0
#define TEST_FILEIO 1

int read_async = 0;

void callback(int num_bytes_read) {
	std::cout << "num_bytes_read: " << num_bytes_read << std::endl;
}

void print_dirs(const filenames_t& filenames) {
	printf("\n");
	for (const auto& filename : filenames) {
		std::cout << filename.data() << std::endl;
	}
}

int main() {

#if TEST_STACK_ALLOC

	stack_alloc_t stack;
	{
		// stack_alloc_t::stack_frame_t frame;
		stack.create_stack_frame();

		int* a_ptr = (int*)stack.allocate(sizeof(int));
		*a_ptr = 5;

		char* b_ptr = stack.allocate(sizeof(char));
		*b_ptr = 0x0a;

		char* c_ptr = stack.allocate(sizeof(char));
		*c_ptr = 0x0b;

		stack.remove_stack_frame();
	}

	{
		// stack_alloc_t::stack_frame_t frame;
		stack.create_stack_frame();

		double* a_ptr = (double*)stack.allocate(sizeof(double));
		*a_ptr = 25;

		char* b_ptr = stack.allocate(sizeof(char));
		*b_ptr = 0x0a;

		stack.remove_stack_frame();
	}


#elif TEST_POOL_ALLOC
	/*
	const int pool_size = 20;
	pool_alloc_t<int, pool_size> int_pool;

	typedef pool_chunk_t<int, pool_size> pct;

	pct* a_ptr = int_pool.allocate();
	*a_ptr->data_ptr = 5;

	pct* b_ptr = int_pool.allocate();
	*b_ptr->data_ptr = 10;

	int_pool.deallocate(b_ptr);
	pct* c_ptr = int_pool.allocate();
	*c_ptr->data_ptr = 20;

	for (int i = 0; i < pool_size - 2; i++) {
		int_pool.allocate();
	}
	*/

	const int pool_size = 10;
	pool_alloc_t<int, pool_size> int_pool;

	typedef pool_chunk_t<int, pool_size> pct;

	int* a_ptr = int_pool.allocate();
	*a_ptr = 5;
	int_pool.deallocate(a_ptr);

	int* b_ptr = int_pool.allocate();
	*b_ptr = 10;

	int* c_ptr = int_pool.allocate();
	*c_ptr = 20;

#elif TEST_SINGLE_FRAME_STACK_ALLOC == 1
	single_frame_alloc_t single_frame_alloc;
	for (int i = 0; i < 2; i++) {
		single_frame_alloc.handle_sof();
		int* a_ptr = (int*)single_frame_alloc.allocate(sizeof(int));
		*a_ptr = 5 * i;
		char* b_ptr = (char*)single_frame_alloc.allocate(sizeof(char));
		*b_ptr = 15 * i;
		double* c_ptr = (double*)single_frame_alloc.allocate(sizeof(double));
		*c_ptr = 25 * i;
		single_frame_alloc.handle_eof();
	}

#elif TEST_DOUBLE_FRAME_STACK_ALLOC == 1
	double_frame_alloc_t double_frame_alloc;
	for (int i = 0; i < 4; i++) {
		double_frame_alloc.handle_sof();
		int* a_ptr = (int*)double_frame_alloc.allocate(sizeof(int));
		*a_ptr = 5 * (i + 1);
		char* b_ptr = (char*)double_frame_alloc.allocate(sizeof(char));
		*b_ptr = 15 * (i + 1);
		double* c_ptr = (double*)double_frame_alloc.allocate(sizeof(double));
		*c_ptr = 25 * (i + 1);
		double_frame_alloc.handle_eof();
	}

#elif TEST_FILEPATH == 1

	// len 61
	filepath_t filepath("C:\\Sarthak\\programming\\SpaceInvaders\\SpaceInvaders\\allocators");
	bool is_valid_path = filepath.is_valid_path();
	filenames_t filenames;
	filepath.list_subdir_and_subfiles(filenames);
	print_dirs(filenames);

	const char new_path[MAX_PATH] = {};
	memcpy((void*)new_path, filenames[0].data(), MAX_PATH);
	char* p = (char*)new_path;
	filepath.append_to_path(new_path);

	filepath.go_to_parent_directory();

	filenames.clear();
	filepath.list_subdir_and_subfiles(filenames);
	print_dirs(filenames);

	filepath.go_to_parent_directory();
	filepath.go_to_parent_directory();
	filepath.go_to_parent_directory();
	filepath.go_to_parent_directory();
	filepath.go_to_parent_directory();

	filepath.go_to_parent_directory();
	filepath.go_to_parent_directory();
	filepath.go_to_parent_directory();

	filenames.clear();
	filepath.list_subdir_and_subfiles(filenames);
	print_dirs(filenames);

	bool is_valid_path_3 = filepath.is_valid_path();

	const char* filename = filepath.get_filename();
	bool is_dir = filepath.is_directory();

#elif TEST_FILEIO == 1

	// const char file_path[] = "C:\\Sarthak\\programming\\SpaceInvaders\\SpaceInvaders\\allocators\\test.txt";
	// const char file_path[] = "C:\\Sarthak\\programming\\VulkanGraphicsEngine\\assets\\viking_room.obj";
	const char file_path[] = "C:\\Sarthak\\programming\\SpaceInvaders\\art\\couch.fbx";
	const char dir_path[] = "C:\\Sarthak\\programming\\SpaceInvaders\\SpaceInvaders\\allocators";
	const char dir_path_2[] = "C:\\Sarthak\\programming\\SpaceInvaders\\SpaceInvaders\\allocators\\test";
	const char dir_path_3[] = "C:\\programming\\a\\SpaceInvaders\\allocators\\test";
	// const char file_path[] = "C:\\Sarthak\\programming\\SpaceInvaders\\art\\pico8_invaders_SPLASH.png";

	// file_io::create_dir(dir_path);
	// file_io::create_dir(dir_path_2);
	// file_io::create_dir(dir_path_3);

	bool success = file_io::create_file(file_path, false);

	int file_size = file_io::get_file_size(file_path);
	char* buffer = new char[file_size];
	memset(buffer, 0, file_size);
	int amount_read = 0;
	// file_io::read_file_sync(file_path, buffer, file_size, amount_read);
	file_io::read_file_async(file_path, buffer, file_size, callback);
	while (!read_async) {}
	// success = file_io::delete_file("C:\\Sarthak\\programming\\SpaceInvaders\\SpaceInvaders\\allocators\\test.txt");


#endif
}