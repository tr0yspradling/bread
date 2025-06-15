cmake_minimum_required(VERSION 3.20)

project(cache_server)

set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-v")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v")

include(FetchContent)

# For Windows: Prevent overriding the parent project's compiler/linker settings
add_library(
    cache_server_lib
    include/bread/cache_server.h
    src/bread/handler.h
    include/bread/command.h
    src/bread/cache_server.cpp
    src/bread/command.cpp
    src/bread/handler.cpp
)

add_library(
    client_lib
    include/bread/cache_client.h
    src/bread/cache_client.cpp
)

set_property(TARGET cache_server_lib PROPERTY PREFIX "")
target_link_libraries(cache_server_lib PUBLIC sockpp)
target_include_directories(
    cache_server_lib
    PUBLIC
        include
        external/sockpp/include
    PRIVATE
        src
)

set_property(TARGET client_lib PROPERTY PREFIX "")
target_link_libraries(client_lib PUBLIC sockpp)
target_include_directories(
    client_lib
    PUBLIC
        include
        external/sockpp/include
    PRIVATE
        src
)

set(SOCKPP_BUILD_IPV6 ON CACHE BOOL "" FORCE)
set(SOCKPP_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SOCKPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(external/sockpp)

include_directories(include src)

add_executable(
    cache_server
    src/bread/main.cpp
)

target_link_libraries(cache_server PUBLIC sockpp)
target_link_libraries(cache_server PUBLIC cache_server_lib)

option(ENABLE_TESTS "Compile & execute unit testing." ON)
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
