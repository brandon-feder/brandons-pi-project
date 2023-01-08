cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(BrandonsPiProgram LANGUAGES CXX CUDA)
enable_language(CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(BPPLib "")
target_sources(BPPLib PRIVATE
    src/moduli-set.cpp
    src/residue-int.cpp
)

target_include_directories(BPPLib PRIVATE src)

# target_compile_features(BPPLib PUBLIC cxx_std_11)
set_target_properties(BPPLib PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
set_target_properties(BPPLib PROPERTIES CUDA_ARCHITECTURES "61")

add_executable(Main src/main.cpp)
set_property(TARGET Main PROPERTY CUDA_SEPARABLE_COMPILATION ON)
target_link_libraries(Main PRIVATE BPPLib m iouring)
set_target_properties(Main PROPERTIES CUDA_ARCHITECTURES "61")