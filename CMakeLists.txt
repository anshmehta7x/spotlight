cmake_minimum_required(VERSION 3.16)

project(spotlight)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)
add_compile_options(-stdlib=libc++)
add_link_options(-stdlib=libc++)

add_executable(indexer
    src/indexer/main.cpp
    src/indexer/file_crawler.cpp
    src/indexer/sqlite_wrapper.cpp
)

target_include_directories(indexer PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

find_package(SQLite3 REQUIRED)

target_link_libraries(indexer
    PRIVATE
        SQLite::SQLite3
)
