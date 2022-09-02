cmake_minimum_required(VERSION 3.10)

set(This TCP_chat_tests)
set(Sources test.cpp)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.11.0
)

FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)

add_executable(string_func_test string_func_tests.cpp)
#add_executable(move_validation_tests move_validation_tests.cpp)
#add_executable(endings_tests endings_tests.cpp)

target_link_libraries(GTest::GTest INTERFACE gtest_main)

target_link_libraries(string_func_test PRIVATE GTest::GTest Chat_funcs)
#target_link_libraries(move_validation_tests PRIVATE GTest::GTest libpcbr)
#target_link_libraries(endings_tests PRIVATE GTest::GTest libpcbr)

enable_testing()

add_test(TCS_sft string_func_test)
#add_test(PCBR_move_validation_tests move_validation_tests)
#add_test(PBCR_endings_tests endings_tests)