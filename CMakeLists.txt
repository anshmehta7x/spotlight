cmake_minimum_required(VERSION 3.16)
project(spotlight)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)
#add_compile_options(-stdlib=libc++)
#add_link_options(-stdlib=libc++)

include_directories(${CMAKE_SOURCE_DIR}/include)

# Find packages
find_package(SQLite3 REQUIRED)
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

set(COMMON_SRC
        src/common/file_crawler.cpp
        src/common/sqlite_wrapper.cpp
)

set(COMMON_HEADERS
        include/sqlite_wrapper.h
        include/ignored_folders.h
)

add_executable(indexer
        src/service/indexer_service.cpp
        ${COMMON_SRC}
        ${COMMON_HEADERS}
        include/util.h
        src/common/trie.cpp
        include/trie.h
)
target_link_libraries(indexer PRIVATE SQLite::SQLite3)

add_executable(search_client
        src/client/client.cpp
        src/client/window.cpp
        ${COMMON_SRC}
        ${COMMON_HEADERS}
        include/util.h
        src/common/trie.cpp
        include/trie.h
        include/client.h
        include/window.h
)

target_link_libraries(search_client PRIVATE
        SQLite::SQLite3
        ${wxWidgets_LIBRARIES}
)