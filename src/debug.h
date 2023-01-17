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

// Creates a file, sets it's size, and returns it's file descriptor
int create_file_of_size(string path, string fname);

void c_close_files(uint32_t n, int *fds);
#endif