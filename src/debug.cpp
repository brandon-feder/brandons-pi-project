#include "debug.h"

using namespace std;

string str_fprintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char *c_str;
    vasprintf(&c_str, format, args);
    va_end(args);
    return string(c_str);
}

void test_env_vars(string env_name)
{
    if(!getenv(env_name.c_str()))
        throw runtime_error(str_fprintf(
            "%s Could not get environment variable \"%s\"\n", 
            ERR, env_name.c_str()));
}

template<typename T>
string vec_str(vector<T> vec)
{
    if(vec.size() == 0) return "[]";
    else
    {
        stringstream ss;
        ss << "[" << vec[0];
        for(uint32_t i = 1; i < vec.size(); ++i)
            ss << ", " << vec[i];
        ss << "]";

        return ss.str();
    }
}

// Returns path object to the nth test file
filesystem::path nth_test_file_path(uint32_t n)
{
    filesystem::path path = {TEST_DIR_PATH};
    stringstream file;
    file << "BBP-TEST-" << n << ".32b";
    path /= file.str();
    return path;
}

// Creates directory if it does not exist
void create_test_dir()
{
    int res = mkdir(TEST_DIR_PATH, 0744);
    if(res < 0 && errno != 17) // Not "file exists error"
        throw runtime_error(str_fprintf(
            "Could not create test directory %s. errno %d: %s\n", 
            TEST_DIR_PATH, errno, strerror(errno)
        ).c_str());
}

// Creates test file (if it does not exist), sets it's length
// to at least len bytes (may be longer if already exists),
// and returns the file. Does not create the directory
ofstream create_test_file_no_dir(uint32_t n, size_t len)
{
    filesystem::path fpath = nth_test_file_path(n);
    ofstream file = ofstream(fpath, ios::binary | ios::out);
    file.seekp(len-1);
    file.write("", 1);
    file.seekp(0);
    file.close();
    return file;
}

// Creates nth test file and directory if needed
ofstream create_test_file(uint32_t n, size_t len)
{
    create_test_dir();
    return create_test_file_no_dir(n, len);
}

// Creates test file directory and test files with "index" 
// 0, 1, ... n-1 all of length at least n. 
// See create_test_file()
void create_n_test_files(uint32_t n, size_t len)
{
    create_test_dir();
    for(uint32_t i = 0; i < n; ++i) 
        create_test_file_no_dir(i, len);
}

// C-Opens (gets fd) the first n test files for reading
// and writing. They must already be created.
void c_open_test_files(uint32_t n, int *fds)
{
    bool failed = false;
    uint32_t i, j = 0;
    for(; i < n; ++i) {
        fds[i] = open(nth_test_file_path(i).c_str(), O_RDWR);
        if(fds[i] < 0) {
            failed = true;
            break;
        }
    }

    if(failed) {
        for(; j <= i; ++j)
            close(fds[i]);

        throw runtime_error(str_fprintf(
            "Could not open file %s. errno %d: %s",
            nth_test_file_path(i), errno, strerror(errno)
        ));
    }
}

void c_close_files(uint32_t n, int *fds) {
    for(uint32_t i = 0; i < n; ++i)
        close(fds[i]);
}
