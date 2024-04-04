# Fetch from git
message(CHECK_START "Fetching Eigen3")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

include(FetchContent)
FetchContent_Declare(
  Eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG 3.4.0
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE)
# note: To disable eigen tests,
# you should put this code in a add_subdirectory to avoid to change
# BUILD_TESTING for your own project too since variables are directory
# scoped
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(BUILD_TESTING OFF)
set(EIGEN_BUILD_TESTING OFF)
set(EIGEN_MPL2_ONLY ON)
set(EIGEN_BUILD_PKGCONFIG OFF)
set(EIGEN_BUILD_DOC OFF)
set(CUDA_USE_STATIC_CUDA_RUNTIME OFF)
FetchContent_MakeAvailable(Eigen)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "fetched")

IF(MSVC AND CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
   ADD_DEFINITIONS(/arch:SSE2)
   message(STATUS "Win32 SSE2 enabled")
ENDIF()
