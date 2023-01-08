#ifndef DEBUG_INCLUDED
#define DEBUG_INCLUDED

#include <string>
#include <sstream>
#include <vector>

using namespace std;

#define SUC "\e[92m[SUC]\e[0m"
#define ERR "\e[91m[ERR]\e[0m"
#define INF "\e[96m[INF]\e[0m"
#define WAR "\e[93m[WAR]\e[0m"

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

#endif