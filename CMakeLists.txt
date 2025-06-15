cmake_minimum_required(VERSION 3.20)

project(cache_server)

set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-v")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v")

include(FetchContent)

# For Windows: Prevent overriding the parent project's compiler/linker settings
add_library(
    bread
    include/bread/cache_server.h
    include/bread/command.h
    src/bread/cache_server.cpp
    src/bread/command.cpp
    src/bread/handler.cpp
    src/bread/handler.h
)

set_property(TARGET bread PROPERTY PREFIX "")
target_link_libraries(bread PUBLIC sockpp)
target_include_directories(
    bread
    PUBLIC
        include
        external/sockpp/include
    PRIVATE
        src/bread
)

set(SOCKPP_BUILD_IPV6 ON CACHE BOOL "" FORCE)
set(SOCKPP_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SOCKPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(external/sockpp)

include_directories(include src/bread)

add_executable(
    cache_server
    src/server/main.cpp
)

target_link_libraries(cache_server PUBLIC bread)

option(ENABLE_TESTS "Compile & execute unit testing." ON)
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
