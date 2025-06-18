cmake_minimum_required(VERSION 3.20)

# Allow specifying the version externally. Default is a dev string.
if(NOT DEFINED BREAD_VERSION)
    if(DEFINED ENV{BREAD_VERSION})
        set(BREAD_VERSION "$ENV{BREAD_VERSION}")
    else()
        set(BREAD_VERSION "0.0.0")
    endif()
endif()

project(cache_server VERSION ${BREAD_VERSION})

add_compile_definitions(BREAD_VERSION="${PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-v")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v")

include(FetchContent)

# Purpose: Place all runtime artifacts in ${CMAKE_BINARY_DIR}/bin/<config>
if (MSVC)
    set(_bin_dir ${CMAKE_BINARY_DIR}/bin)
    foreach(cfg ${CMAKE_CONFIGURATION_TYPES})          # Debug, Release, …
        string(TOUPPER "${cfg}" _ucfg)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_ucfg} "${_bin_dir}")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_ucfg} "${_bin_dir}")
    endforeach()
endif()

# For Windows: Prevent overriding the parent project's compiler/linker settings
add_library(
    bread
    include/bread/cache_server.h
    include/bread/command.h
    include/bread/storage.h
    include/bread/storage_engine.h
    include/bread/storage_expiration.h
    include/bread/commands/bad_command.h
    include/bread/commands/command_factory.h
    include/bread/commands/delete_command.h
    include/bread/commands/get_command.h
    include/bread/commands/quit_command.h
    include/bread/commands/set_command.h
    include/bread/commands/add_command.h
    include/bread/commands/replace_command.h
    include/bread/commands/append_command.h
    include/bread/commands/prepend_command.h
    include/bread/commands/unknown_command.h
    src/bread/cache_server.cpp
    src/bread/commands/command_factory.cpp
    src/bread/commands/set_command.cpp
    src/bread/commands/get_command.cpp
    src/bread/commands/delete_command.cpp
    src/bread/commands/quit_command.cpp
    src/bread/commands/add_command.cpp
    src/bread/commands/replace_command.cpp
    src/bread/commands/append_command.cpp
    src/bread/commands/prepend_command.cpp
    src/bread/commands/unknown_command.cpp
    src/bread/commands/bad_command.cpp
    src/bread/handler.cpp
    src/bread/handler.h
    src/bread/storage.cpp
    src/bread/storage_expiration.cpp
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

# Documentation -------------------------------------------------------------
option(BREAD_BUILD_DOCS "Generate Sphinx docs" OFF)
if(BREAD_BUILD_DOCS)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
    add_custom_target(docs ALL
        COMMAND ${Python3_EXECUTABLE} -m sphinx -b html
                ${CMAKE_CURRENT_SOURCE_DIR}/docs
                ${CMAKE_CURRENT_BINARY_DIR}/docs
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Building documentation with Sphinx"
        VERBATIM)
endif()
