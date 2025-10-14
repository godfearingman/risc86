cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(risc86)

set(SDL2_DIR "${CMAKE_SOURCE_DIR}/src/external/SDL2-2.32.8")
include_directories(${SDL2_DIR}/include/SDL2)
link_directories(${SDL2_DIR}/lib/x64)

file(GLOB_RECURSE SOURCES src/*.cpp)

list(FILTER SOURCES EXCLUDE REGEX "src/external/ELFIO/examples/.*")
list(FILTER SOURCES EXCLUDE REGEX "src/external/ELFIO/tests/.*")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    src/external/ELFIO
)

target_link_libraries(${PROJECT_NAME} 
    SDL2
)


target_compile_definitions(${PROJECT_NAME} PRIVATE SDL_MAIN_HANDLED)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:/Ox /GL /arch:AVX2>
        $<$<CONFIG:Debug>:/Od /Zi>
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:/LTCG>
        /SUBSYSTEM:CONSOLE
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:-O3 -march=native -flto>
        $<$<CONFIG:Debug>:-O0 -g>
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:-flto>
    )
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${SDL2_DIR}/lib/x64/SDL2.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMENT "Copying SDL2.dll to output directory"
)