# Bread

Bread is a simple experimental cache server written in C++23. The project aims to mimic some basic functionality of [memcached](https://memcached.org/) using the `sockpp` networking library.  It is built with CMake and includes minimal unit tests with GoogleTest.

## Directory overview

- `external/` – Third party code used by the project.  It currently contains the `sockpp` submodule for socket wrappers and a header-only copy of `tl_expected.hpp` for `std::expected` compatibility.
- `include/` – Public header files for the cache server.
- `src/` – Implementation sources.  This contains the main executable and supporting classes.
- `tests/` – Unit tests built with GoogleTest.

The top level `CMakeLists.txt` configures the build of the `cache_server` executable and its tests.

## Code formatting

The repository provides a `.clang-format` file based on the Google style. Run
`clang-format` before committing changes:

```bash
clang-format -i src/*.cpp include/*.h tests/*.cpp
```
