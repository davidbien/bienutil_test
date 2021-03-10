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
add_executable (utf_test_direct "utf_test_direct.cpp" "bienutil_test.h")
target_include_directories(utf_test_direct PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/bienutil>
)
target_compile_definitions( utf_test_direct
  PUBLIC __NDEBUG_THROW
)
target_link_libraries(utf_test_direct gtest gtest_main)

gtest_discover_tests(utf_test_direct)

add_executable (utf_test_file "utf_test_file.cpp" "bienutil_test.h")
target_include_directories(utf_test_file PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/bienutil>
)
target_compile_definitions( utf_test_file
  PUBLIC __NDEBUG_THROW
)
target_link_libraries(utf_test_file gtest gtest_main)

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
