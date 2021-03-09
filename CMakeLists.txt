# CMakeList.txt : CMake project for bienutil_test, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
include(FetchContent)
include(GoogleTest)

project ("bienutil_test")

set(GTEST_VERSION 1.10.0 CACHE STRING "Google test version")
FetchContent_Declare(googletest
GIT_REPOSITORY https://github.com/google/googletest.git
GIT_TAG release-${GTEST_VERSION})
FetchContent_MakeAvailable(googletest)
include_directories("${gtest_SOURCE_DIR}/include")

enable_testing()

include ( bienutil/bienutil.cmake )

# create an output directory where we will create all of our unittest files based on the template unittest files that are in the source tree.
add_custom_target(make-build-unittest-dir ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "unittests")

# Add source to this project's executable.
add_executable (bienutil_test "bienutil_test.cpp" "bienutil_test.h")

target_include_directories(bienutil_test PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/bienutil>
)
target_compile_definitions( bienutil_test
  PUBLIC __NDEBUG_THROW
)

gtest_discover_tests(bienutil_test)

# TODO: Add tests and install targets if needed.
