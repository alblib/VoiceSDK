cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0077 NEW)
cmake_policy(SET CMP0091 NEW)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON) #Required to Windows dll have the symbol for entry points

project(VoiceSDK)

option(VoiceSDK_SHARED_LIB "Build VoiceSDK shared library" OFF)
option(VoiceSDK_C_API "Build VoiceSDK C functions" OFF)
option(BUILD_VoiceSDK_TESTS "Makes test executable project of VoiceSDK by adding main.cpp in the build folder" OFF)


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

if(VoiceSDK_SHARED_LIB)
  add_library(VoiceSDK SHARED ${VoiceSDK_HEADERS} ${VoiceSDK_SOURCES})
else()
  add_library(VoiceSDK STATIC ${VoiceSDK_HEADERS} ${VoiceSDK_SOURCES})
endif()

target_include_directories(VoiceSDK PRIVATE src)
set_target_properties(
  VoiceSDK PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/src
    LINKER_LANGUAGE CXX # .hpp is not recognized maybe
)
target_link_libraries(VoiceSDK PRIVATE Eigen3::Eigen)

if (BUILD_VoiceSDK_TESTS)
  if (NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/src/main.cpp")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/src/main.cpp" "int main(){\n    return 0;\n}")
  endif()
  add_executable("VoiceSDK_test" "${CMAKE_CURRENT_BINARY_DIR}/src/main.cpp")
  target_link_libraries("VoiceSDK_test" PRIVATE VoiceSDK)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "VoiceSDK_test")
endif()