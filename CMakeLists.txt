cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0077 NEW)
cmake_policy(SET CMP0091 NEW)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON) #Required to Windows dll have the symbol for entry points

project(VoiceSDK)

# option(VoiceSDK_SHARED_LIB "Build VoiceSDK shared library" OFF)
option(VoiceSDK_C_API "Build VoiceSDK C functions" OFF)

# Testing
#include(CTest)
option(VoiceSDK_TESTS "Tests VoiceSDK" ON)

if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
    add_compile_options("/Zc:__cplusplus")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(cmake/FindEigen.cmake)

file(GLOB_RECURSE VoiceSDK_SOURCES 
  ${PROJECT_SOURCE_DIR}/src/*.cpp 
  ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE VoiceSDK_HEADERS 
  ${PROJECT_SOURCE_DIR}/src/*.h 
  ${PROJECT_SOURCE_DIR}/src/*.hpp)

# if(VoiceSDK_SHARED_LIB)
#   add_library(VoiceSDK SHARED ${VoiceSDK_HEADERS} ${VoiceSDK_SOURCES})
# else()
#   add_library(VoiceSDK STATIC ${VoiceSDK_HEADERS} ${VoiceSDK_SOURCES})
# endif()
add_library(VoiceSDK INTERFACE)

target_include_directories(VoiceSDK SYSTEM INTERFACE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(VoiceSDK INTERFACE Eigen3::Eigen)

set_target_properties(
  VoiceSDK PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/src
    LINKER_LANGUAGE CXX # .hpp is not recognized maybe
)
# target_link_libraries(VoiceSDK PRIVATE Eigen3::Eigen)


# Testing
#if(BUILD_TESTING)
if(VoiceSDK_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()