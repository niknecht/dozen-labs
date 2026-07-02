cmake_minimum_required(VERSION 3.30)

project(subway VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_STANDARD_REQUIRED 23)
set(CMAKE_CXX_STANDARD 23)
set(CXXFLAGS -std=c++23 -fuse-ld=lld -Wall -Wextra -Wpedantic -Winfo -Werror)

option(CompileTests "Compile optional unit tests into a separate executable." ON)
option(MakeDocs "Compile doxygen documentation pages." OFF)

add_subdirectory(model)
add_subdirectory(view)
add_subdirectory(viewmodel)

if(CompileTests)
	add_subdirectory(test)
endif()


