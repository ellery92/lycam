project(lycam)
cmake_minimum_required(VERSION 3.11)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(src/3rdparty/poco)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
add_subdirectory(src)

add_subdirectory(test)
