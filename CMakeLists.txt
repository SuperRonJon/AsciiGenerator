cmake_minimum_required(VERSION 3.25)
project(AsciiGenerator LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(asciigen src/asciigen.c)
target_compile_features(asciigen PRIVATE c_std_99)
if(NOT WIN32)
    target_link_libraries(asciigen PRIVATE m)
endif()
