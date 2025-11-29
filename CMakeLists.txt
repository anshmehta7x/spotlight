cmake_minimum_required(VERSION 3.16)
project(spotlight)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

add_executable(indexer
    src/indexer/main.cpp
    src/indexer/file_crawler.cpp
)

target_link_libraries(indexer c++ c++abi)
