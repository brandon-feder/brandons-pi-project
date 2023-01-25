#ifndef __DEBUG_DEFS__
#include "debug.h"

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

off_t get_file_size(int fd) {
    struct stat st;

    if (fstat(fd, &st) < 0 )
        throw runtime_error(str_fprintf(
            "Could not get file stats: %s",
            strerror(errno)
        ));

    if(S_ISREG(st.st_mode)) {
        return st.st_size;
    } else if (S_ISBLK(st.st_mode)) {
        unsigned long long bytes;

        if (ioctl(fd, BLKGETSIZE64, &bytes) != 0)
            throw runtime_error(str_fprintf(
                "Could not get block device size: %s",
                strerror(errno)
            ));

        return bytes;
    }
    
    throw runtime_error(str_fprintf("Could not get device size %s"));
}

#else 

template<typename T>
string vec_to_str(vector<T> vec)
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

#endif