# Repository Guidelines

## Code Formatting
- Apply `clang-format` using the provided configuration before committing. Run:
  ```bash
  clang-format -i src/*.cpp include/*.h tests/*.cpp
  ```
- Ensure any new source files are added to the build system.

## Testing
- Compile and run the test suite with CMake:
  ```bash
  cmake -B build -S .
  cmake --build build
  ctest --test-dir build --output-on-failure
  ```
  Ensure all tests pass before submitting a PR.

- For additional checks, run static analysis tools such as `clang-tidy` or `cppcheck` when possible.

## Documentation
- Build and verify the Sphinx documentation before pushing changes:
  ```bash
  pip install -r docs/requirements.txt
  ./docs/update_docs.sh
  ```

## Security and Issue Management
- Follow the process outlined in `SECURITY.md` to report vulnerabilities.
- Use GitHub issues to track bugs and feature requests. Reference the issue number in your commits and PR descriptions.

## Branching and Sign-Off
- Create descriptive feature branches like `feature/<slug>` or `bugfix/<slug>`.
- Sign your commits using `git commit -s` to add a `Signed-off-by` line.

## Pull Request Expectations
- Summarize the implemented changes and reference any modified files in the PR body.
- Include a short note about the result of running the test suite.
