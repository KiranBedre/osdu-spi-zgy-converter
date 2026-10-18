# General

- It is good practice to reach out and start a discussion before opening a pull request for a significant change. It can save a lot of unwanted emotions for all parties.
- Exported methods are marked with `SEGYSDK_DLL_PUBLIC`. Keep the API/ABI stable, unless change is inevitable.
- Don't accumulate tech debt. Add proper unit tests (even if it looks hard at first). If test data is needed, keep it to the minimum required size (e.g., if a piece of logic can be tested with a 3x3x3 cube then don't add a 100MB SEG-Y file).

# Coding conventions

## C++

- We recommend to use an IDE with built-in CMake support (Visual Studio 2019+, Qt Creator, CLion...)
- Use clang-format
    - Use an IDE with clang-format integration
    - The project has a .clang-format configuration file
    - CLang tools version 11 or above are required for working properly
    - For Visual Studio 2019, we recommend to use an extension to automatically format the file on each save
    - Or configure a pre-commit hook for git
- Use common sense
    - Not everything is specified in .clang-format
    - If a file was written in a specific way, try to stick to the existing style, unless there is good reason not to do so
    - Don't be afraid to change/introduce a new rule after appropriate discussion
    - Commit formatting changes (if any) in separate commits, don't mix with logic changes

## Python

- Follow common Python practices (e.g., PEP-8)
