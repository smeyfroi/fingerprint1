# Agent Guidelines for fingerprint1 OpenFrameworks Project

## Build/Test Commands
- **Build**: `make` or `make Release` (builds to bin/ directory)
- **Debug Build**: `make Debug` 
- **Clean**: `make clean`
- **Run**: Execute the binary in `bin/` or open Xcode project and run
- **No unit tests** - OpenFrameworks apps typically test through visual output

## Code Style Guidelines
- **Language**: Modern C++ (C++11/17 compatible with macOS 10.15+)
- **Headers**: Use `#pragma once`, include `ofMain.h` first, then custom headers
- **Imports**: System includes first, then OF includes, then custom addon includes
- **Naming**: camelCase for variables/functions, PascalCase for classes, ALL_CAPS for constants
- **Types**: Use `std::shared_ptr` for shared resources, `constexpr` for compile-time constants
- **Formatting**: 2-space indentation, braces on new lines for functions/classes
- **Error Handling**: Use OF logging (`ofLog()`) and graceful degradation
- **Memory**: Prefer RAII, smart pointers, and OF's automatic resource management
- **File Paths**: Use `std::filesystem::path` for cross-platform compatibility
- **Override**: Always use `override` keyword for virtual function implementations
- **Constants**: Define configuration constants at top of header files with clear comments

## Project Structure
- Main app logic in `ofApp.cpp/h`, entry point in `main.cpp`
- Uses multiple addons - check `addons.make` before adding new dependencies
- Audio analysis integration with custom addon `ofxMarkSynth`