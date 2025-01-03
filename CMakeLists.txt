cmake_minimum_required(VERSION 3.29)

project(cache_server)

set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-v")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v")

include(FetchContent)

# For Windows: Prevent overriding the parent project's compiler/linker settings
add_library(
    cache_server_lib
    include/cache_server.h
    src/handler.h
    include/command.h
    src/cache_server.cpp
    src/command.cpp
    src/handler.cpp
)

set_property(TARGET cache_server_lib PROPERTY PREFIX "")
target_include_directories(cache_server_lib PRIVATE dependencies/sockpp/include)
target_include_directories(cache_server_lib PRIVATE include)

set(SOCKPP_BUILD_IPV6 ON CACHE BOOL "" FORCE)
set(SOCKPP_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SOCKPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(dependencies/sockpp)

include_directories(include)

add_executable(
    cache_server
    src/main.cpp
)

target_link_libraries(cache_server PUBLIC sockpp)
target_link_libraries(cache_server PUBLIC cache_server_lib)

option(ENABLE_TESTS "Compile & execute unit testing.")
IF (ENABLE_TESTS)
ENDIF()
add_subdirectory(tests)
