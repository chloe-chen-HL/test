cmake_minimum_required(VERSION 3.14)
project(StateBaseTests LANGUAGES CXX)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(core
  src/statebase.cpp
)

target_include_directories(core PUBLIC include)

add_executable(statebase_tests
  tests/test_statebase.cpp
)

target_link_libraries(statebase_tests PRIVATE core GTest::gtest GTest::gtest_main)

include(GoogleTest)

gtest_discover_tests(statebase_tests)