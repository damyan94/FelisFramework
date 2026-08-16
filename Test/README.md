# Felis tests

The tests use a deliberately small hand-written reporter. There is no external
test framework. CMake registers the test executable with CTest for native builds
and cross-compilation builds that provide `CMAKE_CROSSCOMPILING_EMULATOR`.

## Building

From the repository root:

```bash
cmake -S . -B Build -DFELIS_BUILD_TEST=ON
cmake --build Build -j4
```

## Running

```bash
ctest --test-dir Build --output-on-failure
```

For multi-configuration generators, select the configuration as well:

```bash
ctest --test-dir Build -C Debug --output-on-failure
```

Cross-compiled tests without an emulator must be copied to and run on the target
platform, such as an Android device.

The current suites cover:

- application lifecycle behavior;
- command-line arguments;
- errors and error registries;
- logging;
- time and duration utilities;
- timers and timer handles.

Suites are called explicitly from `TestApplication::OnRun()`. New suites should
follow the existing test reporter and manual registration convention.

Each suite prints its detailed summary to the console. The executable returns
zero when every check passes and one when any check fails. Application lifecycle
errors retain their existing nonzero application error codes.
