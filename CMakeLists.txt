cmake_minimum_required(VERSION 3.20)

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
target_include_directories(cache_server_lib PRIVATE external/sockpp/include)
target_include_directories(cache_server_lib PRIVATE include)

set(SOCKPP_BUILD_IPV6 ON CACHE BOOL "" FORCE)
set(SOCKPP_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SOCKPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(external/sockpp)

include_directories(include)

add_executable(
    cache_server
    src/main.cpp
)

target_link_libraries(cache_server PUBLIC sockpp)
target_link_libraries(cache_server PUBLIC cache_server_lib)

option(ENABLE_TESTS "Compile & execute unit testing." ON)
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Documentation -------------------------------------------------------------
option(BREAD_SKIP_DOCS "Skip generating Sphinx docs" OFF)
if(NOT BREAD_SKIP_DOCS)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
    add_custom_target(docs ALL
        COMMAND ${Python3_EXECUTABLE} -m sphinx -b html
                ${CMAKE_CURRENT_SOURCE_DIR}/docs
                ${CMAKE_CURRENT_BINARY_DIR}/docs
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Building documentation with Sphinx"
        VERBATIM)
endif()
