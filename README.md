# Felis Framework

Felis Framework is a personal, general-purpose C++23 foundation for building
all kinds of C++ applications - CLI tools, UI applications, games and others.

## Current features

- `Application` with an `Init → Run → Deinit` lifecycle;
- named and positional command-line argument parsing;
- safe, non-throwing typed argument conversion;
- enum-based recoverable errors;
- configurable console logging;
- time and duration utilities;
- main-thread timer management with generation-based handles.

Felis intentionally uses precompiled headers. Standard-library headers are
centralized in `Felis/stdafx.h`.

The framework is designed and tested on Linux. Windows compatibility is
desired, but is not currently tested or expected to work out of the box.

## Requirements

- CMake 3.25 or newer;
- a compiler and standard library with C++23 support.

## Building

Build the library:

```bash
cmake -S . -B Build
cmake --build Build -j4
```

Alternatively:

```bash
./build.sh
```

Enable the example and tests when configuring:

```bash
cmake -S . -B Build -DFELIS_BUILD_EXAMPLE=ON -DFELIS_BUILD_TEST=ON
cmake --build Build -j4
```

Run them with:

```bash
./Build/FelisExample --help
./Build/FelisTest
```

Further information:

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
target_link_libraries(MyApplication PRIVATE FelisFramework)
```

Applications derive from `Application` and implement `OnInit()`, `OnRun()` and
`OnDeinit()`. Command-line arguments are available through
`GetCommandLineArguments()`.

Installation, package export and automated test registration are deliberate
future concerns and are not currently provided.

## License

See [LICENSE.md](LICENSE.md).

Damyan Damyanov © 2026
