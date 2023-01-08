#ifndef MISC_CU
#define MISC_CU

#include <cuda_runtime.h>
#include <cuda.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cufile.h>

using namespace std;

// template<class T, typename enable_if<is_integral<T>::value, nullptr_t>::type = nullptr>
// std::string cuFileGetErrorString(T status) {
//    status = std::abs(status);
//    return IS_CUFILE_ERR(status) ?
//             std::string(CUFILE_ERRSTR(status)) : std::string(std::strerror(status));
// }

#define posixCheck(ans) { posixAssert((ans), __FILE__, __LINE__); }
#define cudaCheck(ans) { cudaAssert((ans), __FILE__, __LINE__); }
#define cuFileCheck(ans) { cuFileAssert((ans), __FILE__, __LINE__); }

inline void posixAssert(int code, const char *file, int line, bool abort=true)
{
   if(code < 0) 
   {
      stringstream err;
      err << strerror(errno) << " ";
      err << file << " ";
      err << line;

      throw runtime_error(err.str());
   }
}

inline void cudaAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
   if (code != cudaSuccess) 
   {
      stringstream err;
      err << cudaGetErrorString(code) << " ";
      err << file << " ";
      err << line;

      throw runtime_error(err.str());
   }
}

inline void cuFileAssert(CUfileError_t code, const char *file, int line, bool abort=true)
{
   if (code.err != CU_FILE_SUCCESS) 
   {
      stringstream err;
      err << CUFILE_ERRSTR(code.err) << " ";
      err << file << " ";
      err << line;

      throw runtime_error(err.str());
   }
}


#endif