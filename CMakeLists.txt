cmake_minimum_required(VERSION 3.21)

project(VoiceSDK)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON) #Required to Windows dll have the symbol for entry points

if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
    add_compile_options("/Zc:__cplusplus")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(cmake/FindEigen.cmake)

file(GLOB_RECURSE SOURCES src/*)

add_library(VoiceSDK ${SOURCES})

target_include_directories(VoiceSDK PRIVATE src)
set_target_properties(VoiceSDK PROPERTIES INTERFACE_INCLUDE_DIRECTORIES src)