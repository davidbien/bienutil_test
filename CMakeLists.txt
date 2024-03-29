﻿# CMakeList.txt : CMake project for bienutil_test, include source and define
# project specific logic here.
#
# dbien: Review: Actually we don't end up using the below.
# CMake projects should use: "-DCMAKE_TOOLCHAIN_FILE=/mnt/c/devenv/VcPkgForLinux/vcpkg/scripts/buildsystems/vcpkg.cmake"

cmake_minimum_required (VERSION 3.8)
include(FetchContent)
include(GoogleTest)

project ("bienutil_test")

enable_testing()

set( MOD_USE_VCPKG 1 )
set( MOD_USE_ICU4C 1 )
include ( bienutil/bienutil.cmake )

set(GTEST_VERSION 1.12.0 CACHE STRING "Google test version")
FetchContent_Declare(googletest
GIT_REPOSITORY https://github.com/google/googletest.git
GIT_TAG release-${GTEST_VERSION})
FetchContent_MakeAvailable(googletest)
include_directories("${gtest_SOURCE_DIR}/include")

# create an output directory where we will create all of our unittest files based on the template unittest files that are in the source tree.
add_custom_target(make-build-unittest-dir ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "unittests")

# Add source to this project's executable.
add_executable (utf_test_direct "utf_test_direct.cpp" "bienutil_test.h")
target_include_directories(utf_test_direct PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/bienutil>
)
target_compile_definitions( utf_test_direct
  PUBLIC __NDEBUG_THROW
)
target_link_libraries(utf_test_direct gtest gtest_main ICU::dt ICU::uc)

gtest_discover_tests(utf_test_direct)

add_executable (utf_test_file "utf_test_file.cpp" "bienutil_test.h")
target_include_directories(utf_test_file PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/bienutil>
)
target_compile_definitions( utf_test_file
  PUBLIC __NDEBUG_THROW
)
target_link_libraries(utf_test_file gtest gtest_main  ICU::dt ICU::uc)

# We may not notice new unittests until CMakeLists.txt is rebuilt - that's ok.
file(GLOB UTFUNITTESTS LIST_DIRECTORIES true unittests/*.txt )
foreach( UTFTEST ${UTFUNITTESTS} )
get_filename_component( UTFTEST_NAME ${UTFTEST} NAME_WE )
message("Unit test: ${UTFTEST_NAME}")    
gtest_discover_tests(utf_test_file
                    EXTRA_ARGS  ${UTFTEST}
                    TEST_SUFFIX .${UTFTEST_NAME}
)
endforeach()

# LogArray testing:
add_executable (logarray_test "logarray_test.cpp" "bienutil_test.h")
target_include_directories(logarray_test PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/bienutil>
)
target_compile_definitions( logarray_test
  PUBLIC __NDEBUG_THROW
)
target_link_libraries(logarray_test gtest gtest_main  ICU::dt ICU::uc)
gtest_discover_tests(logarray_test)

# SharedPtr testing:
add_executable (shared_obj_test "shared_obj_test.cpp" "bienutil_test.h")
target_include_directories(shared_obj_test PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/bienutil>
)
target_compile_definitions( shared_obj_test
  PUBLIC __NDEBUG_THROW
)
target_link_libraries(shared_obj_test gtest gtest_main  ICU::dt ICU::uc)
gtest_discover_tests(shared_obj_test)

# SharedWeakPtr testing:
add_executable (shared_weak_test "shared_weak_test.cpp" "bienutil_test.h")
target_include_directories(shared_weak_test PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/bienutil>
)
target_compile_definitions( shared_weak_test
  PUBLIC __NDEBUG_THROW
)
target_link_libraries(shared_weak_test gtest gtest_main  ICU::dt ICU::uc)
gtest_discover_tests(shared_weak_test)

