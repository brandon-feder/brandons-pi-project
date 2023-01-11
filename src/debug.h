#ifndef __DEBUG_INCLUDED__
#define __DEBUG_INCLUDED__

#include <string>
#include <sstream>
#include <vector>
#include <fstream> 
#include <string.h>
#include <filesystem>
#include <sstream>
#include <sys/stat.h>
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

#define SUC "\e[92m[SUC]\e[0m"
#define ERR "\e[91m[ERR]\e[0m"
#define INF "\e[96m[INF]\e[0m"
#define WAR "\e[93m[WAR]\e[0m"
#define TAB "     "

const char *const TEST_DIR_PATH = getenv("BPP_TEST_DIR_PATH");

string str_fprintf(const char *format, ...);
void test_env_vars(string env_name);

template<typename T>
string vec_str(vector<T> vec);

// Returns path object to the nth test file
filesystem::path nth_test_file_path(uint32_t n);

// Creates directory if it does not exist
void create_test_dir();

// Creates test file (if it does not exist), sets it's length
// to at least len bytes (may be longer if already exists),
// and returns the file. Does not create the directory
ofstream create_test_file_no_dir(uint32_t n, size_t len);

// Creates nth test file and directory if needed
ofstream create_test_file(uint32_t n, size_t len);

// Creates test file directory and test files with "index" 
// 0, 1, ... n-1 all of length at least n. 
// See create_test_file()
void create_n_test_files(uint32_t n, size_t len);

// C-Opens (gets fd) the first n test files for reading
// and writing. They must already be created.
void c_open_test_files(uint32_t n, int *fds);

void c_close_files(uint32_t n, int *fds);
#endif