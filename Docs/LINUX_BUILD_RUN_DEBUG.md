# FelisFramework - Linux Build, Run and Debug

Concise notes for building the FelisFramework static library, example and tests
on Linux, running the executables, and debugging them with GDB.

## 1. Install the host tools

On Ubuntu-based distributions:

```bash
sudo apt install build-essential cmake gdb
```

Ninja is an optional personal preference:

```bash
sudo apt install ninja-build
```

Check the installed tools:

```bash
cmake --version
c++ --version
gdb --version
```

The compiler and standard library must support the C++23 features used by the
project.

## 2. List the available presets

From the project root:

```bash
cmake --list-presets
```

The Linux presets are:

```text
linux-debug
linux-release
```

Both presets build the static library, example and tests.

## 3. Configure and build Debug

```bash
cmake --preset linux-debug
cmake --build --preset linux-debug
```

The generated files are placed under:

```text
Build/Linux/Debug/
```

Important artifacts:

```text
Build/Linux/Debug/Source/libFelisFramework.a
Build/Linux/Debug/Example/FelisExample
Build/Linux/Debug/Test/FelisTest
```

The equivalent configuration without presets is:

```bash
cmake -S . -B Build/Linux/Debug \
    -DCMAKE_BUILD_TYPE=Debug \
    -DFELIS_BUILD_EXAMPLE=ON \
    -DFELIS_BUILD_TEST=ON
```

To select Ninja explicitly when first configuring:

```bash
cmake --preset linux-debug -G Ninja
```

## 4. Run the tests

Run through CTest:

```bash
ctest --preset linux-debug
```

Run the executable directly:

```bash
Build/Linux/Debug/Test/FelisTest
echo "Exit code: $?"
```

CTest stores its latest detailed log under:

```text
Build/Linux/Debug/Testing/Temporary/LastTest.log
```

## 5. Run the example

```bash
Build/Linux/Debug/Example/FelisExample
```

## 6. Debug with GDB

Start the example under GDB:

```bash
gdb Build/Linux/Debug/Example/FelisExample
```

Useful commands:

```text
break main              # set a breakpoint
run                     # start the process
next                    # step over
step                    # step into
continue                # continue execution
backtrace               # show the call stack
info locals             # show local variables
print variable          # print an expression
quit                    # exit GDB
```

The tests can be debugged in the same way:

```bash
gdb Build/Linux/Debug/Test/FelisTest
```

## 7. Configure and build Release

Debug and Release use separate build directories:

```bash
cmake --preset linux-release
cmake --build --preset linux-release
ctest --preset linux-release
```

Release artifacts are placed under:

```text
Build/Linux/Release/
```

## 8. Quick development loop

```bash
cmake --build --preset linux-debug &&
ctest --preset linux-debug
```

Reconfiguration is normally only needed after changing CMake settings or
switching to a different preset.

## 9. Minimal reminder

```bash
cmake --preset linux-debug
cmake --build --preset linux-debug
ctest --preset linux-debug
Build/Linux/Debug/Example/FelisExample
```
