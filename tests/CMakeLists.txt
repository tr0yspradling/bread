FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    cache_server_test
    test_command.cpp
)

target_link_libraries(
    cache_server_test
    GTest::gtest_main
    cache_server_lib
)

include(GoogleTest)
gtest_discover_tests(cache_server_test)
