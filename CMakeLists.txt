cmake_minimum_required(VERSION 3.20)

# set the C standard to C++23.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(risc86)

# include the files in the source tree.
file(GLOB_RECURSE SOURCES src/*.cpp)

list(FILTER SOURCES EXCLUDE REGEX "src/external/ELFIO/examples/.*")
list(FILTER SOURCES EXCLUDE REGEX "src/external/ELFIO/tests/.*")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    src/external/ELFIO
)

