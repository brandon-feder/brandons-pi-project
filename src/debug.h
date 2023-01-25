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
#include <sys/ioctl.h>
#include <linux/fs.h>

using namespace std;

#define SUC "\e[92m[SUC]\e[0m"
#define ERR "\e[91m[ERR]\e[0m"
#define INF "\e[96m[INF]\e[0m"
#define WAR "\e[93m[WAR]\e[0m"
#define TAB "     "

const char *const TEST_DIR_PATH = getenv("BPP_TEST_DIR_PATH");

string str_fprintf(const char *format, ...); 
void test_env_vars(string env_name);
off_t get_file_size(int fd);

template<typename T>
string vec_to_str(vector<T> vec);

#define __DEBUG_DEFS__
#include "debug.cpp"

#endif