# Felis Framework

Felis Framework is a personal, general-purpose C++23 foundation for building
all kinds of C++ applications - CLI tools, UI applications, games and others.

## Current features

- `Felis::Application` with an `Init → Run → Deinit` lifecycle;
- named and positional command-line argument parsing;
- safe, non-throwing typed argument conversion;
- enum-based recoverable errors;
- configurable console logging;
- time and duration utilities;
- main-thread timer management with generation-based handles.

Felis intentionally uses precompiled headers. Standard-library headers are
centralized in `Felis/stdafx.h`.

## Supported platforms

FelisFramework is currently built and tested on:

- Linux;
- Android ARM64, API 26 and newer.

Windows 10 and newer and Emscripten are planned targets, but are not yet
documented or verified.

## Requirements

- CMake 3.25 or newer;
- a compiler and standard library with C++23 support.

## Building

The project provides CMake presets that place each platform and configuration
in a separate directory under `Build/`.

For a Linux Debug build:

```bash
cmake --preset linux-debug
cmake --build --preset linux-debug
ctest --preset linux-debug
```

The presets build the static library, example and tests.

Detailed platform instructions:

- [Linux build, run and debug](Docs/LINUX_BUILD_RUN_DEBUG.md)
- [Android build, run and debug](Docs/ANDROID_BUILD_RUN_DEBUG.md)

Component documentation:

- [Example application](Example/README.md)
- [Tests](Test/README.md)

## Using Felis

Felis can currently be consumed directly from its source tree:

```cmake
add_subdirectory(path/to/FelisFramework)

add_executable(MyApplication
    Source/Main.cpp
    Source/stdafx.cpp
)

target_precompile_headers(MyApplication PRIVATE Source/stdafx.h)
target_link_libraries(MyApplication PRIVATE Felis::Framework)
```

The public C++ API is contained in the `Felis` namespace. A consuming project
may introduce a project-local namespace alias without changing the underlying
types or symbols:

```cpp
namespace Pantera = ::Felis;
Pantera::Timer timer;
```

Applications derive from `Felis::Application` and implement `OnInit()`, `OnRun()` and
`OnDeinit()`. Command-line arguments are available through
`GetCommandLineArguments()`.

Installation and package export are deliberate future concerns and are not
currently provided. When tests are enabled, runnable native or emulated test
executables are registered with CTest.

## License

See [LICENSE.md](LICENSE.md).

Damyan Damyanov © 2026
