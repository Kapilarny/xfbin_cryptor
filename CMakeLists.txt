cmake_minimum_required(VERSION 3.31)
project(xfbin_cryptor)

set(CMAKE_CXX_STANDARD 20)

# disable winpthread library on Windows
set(CMAKE_USE_WIN32_THREADS_INIT OFF)

add_subdirectory(external/nucc-cpp)

add_executable(xfbin_cryptor src/main.cpp)
target_link_libraries(xfbin_cryptor PRIVATE
    nucc
    -static-libgcc -static-libstdc++
)
