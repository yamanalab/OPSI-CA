cmake_minimum_required(VERSION 2.8)

set(project_name opsica_demo)
project(${project_name})

find_package(Threads REQUIRED)
set(CMAKE_CXX_FLAGS "-O3 -std=c++11 -pthread -Wall -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O2 -g -std=c++11 -pthread -Wall")

set(HELIB_DIR /usr/local/helib_pack)
include_directories(${PROJECT_SOURCE_DIR}/opsica ${PROJECT_SOURCE_DIR}/stdsc ${HELIB_DIR}/include)
link_directories(${HELIB_DIR}/lib)

set(COMMON_LIBS opsica_share stdsc helib ntl gmp)

add_subdirectory(stdsc)
add_subdirectory(opsica)
add_subdirectory(demo)
