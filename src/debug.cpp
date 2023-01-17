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