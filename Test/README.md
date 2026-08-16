# Felis tests

The tests use a deliberately small hand-written reporter. There is no external
test framework and no automatic test registration.

## Building

From the repository root:

```bash
cmake -S . -B Build -DFELIS_BUILD_TEST=ON
cmake --build Build -j4
```

## Running

```bash
./Build/FelisTest
```

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
