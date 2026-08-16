# FelisFramework - Android Build, Run and Debug

Concise notes for building FelisFramework for Android on Linux, running native executables on a physical Android device, and debugging them remotely with LLDB.

Tested on 2026-08-16 with:

- Linux Mint / Ubuntu-based Linux
- Android SDK Command-line Tools 22
- Android Platform 36
- Build Tools 36.0.0
- Platform Tools 37.0.1
- Android NDK r29 (`29.0.14206865`)
- ARM64 Android device
- Minimum Android API: 26

---

## 1. Install host tools

```bash
sudo apt install build-essential cmake unzip openjdk-17-jdk
```

Ninja is an optional personal preference:

```bash
sudo apt install ninja-build
```

Download the Android **Command line tools only** package for Linux and extract it so the layout is:

```text
~/Dev/0_libs/android-sdk/
└── cmdline-tools/
    └── latest/
        ├── bin/
        ├── lib/
        └── ...
```

---

## 2. Configure `~/.bashrc`

```bash
export ANDROID_HOME="$HOME/Dev/0_libs/android-sdk"
export ANDROID_NDK="$ANDROID_HOME/ndk/29.0.14206865"

export PATH="$PATH:$ANDROID_HOME/cmdline-tools/latest/bin"
export PATH="$PATH:$ANDROID_HOME/platform-tools"
```

Reload:

```bash
source ~/.bashrc
```

Check:

```bash
echo "$ANDROID_HOME"
echo "$ANDROID_NDK"
android --version
adb version
```

---

## 3. Install Android SDK / NDK packages

Using the modern Android CLI:

```bash
android sdk install platform-tools
android sdk install platforms/android-36
android sdk install build-tools/36.0.0
android sdk install ndk/29.0.14206865
```

Check installed packages:

```bash
android sdk list
```

Useful environment info:

```bash
android info
```

If these versions become outdated, use `android sdk list`, install the desired
versions, and update `ANDROID_NDK`.

---

## 4. Configure FelisFramework for Android ARM64

From the project root:

```bash
cmake --preset android-arm64-debug
```

The preset configures an ARM64 Debug build under:

```text
Build/Android/arm64-v8a/Debug/
```

It enables the static library, example and tests, and supplies the NDK toolchain,
ABI and minimum API level.

The equivalent configuration without presets is:

```bash
cmake -S . -B Build/Android/arm64-v8a/Debug \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK/build/cmake/android.toolchain.cmake" \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-26 \
    -DFELIS_BUILD_EXAMPLE=ON \
    -DFELIS_BUILD_TEST=ON
```

To select Ninja explicitly when first configuring:

```bash
cmake --preset android-arm64-debug -G Ninja
```

Build:

```bash
cmake --build --preset android-arm64-debug
```

`ANDROID_PLATFORM=android-26` is the minimum supported Android API level, not the installed SDK platform version.

For another ABI, add a separate preset with its own build directory.

Example for an x86-64 emulator:

```bash
-DANDROID_ABI=x86_64
```

---

## 5. Verify the generated binary

Example:

```bash
file Build/Android/arm64-v8a/Debug/Test/FelisTest
```

For an ARM64 Android build it should report something similar to:

```text
ELF 64-bit LSB pie executable, ARM aarch64, ...
interpreter /system/bin/linker64
```

---

## 6. Prepare a physical Android phone

Enable:

```text
Settings
→ Developer options
→ USB debugging
```

On Samsung devices, if USB debugging is blocked by **Auto Blocker**, temporarily disable:

```text
Settings
→ Security and privacy
→ Auto Blocker
```

Connect the phone by USB and run:

```bash
adb devices
```

Approve the USB debugging authorization prompt on the phone.

A connected device should appear as:

```text
<serial>    device
```

---

## 7. Run a native executable on the phone

Upload:

```bash
adb push Build/Android/arm64-v8a/Debug/Test/FelisTest /data/local/tmp/FelisTest
```

Make executable:

```bash
adb shell chmod +x /data/local/tmp/FelisTest
```

Run:

```bash
adb shell /data/local/tmp/FelisTest
```

Run and print its exit code:

```bash
adb shell '/data/local/tmp/FelisTest; echo "Exit code: $?"'
```

The executable runs on the phone's CPU. `stdout` and `stderr` are sent back through ADB and displayed in the Linux terminal.

Useful checks:

```bash
adb shell getprop ro.product.cpu.abi
adb shell getprop ro.build.version.sdk
```

---

## 8. Quick build + deploy + run loop

```bash
cmake --build --preset android-arm64-debug &&
adb push Build/Android/arm64-v8a/Debug/Test/FelisTest /data/local/tmp/FelisTest &&
adb shell chmod +x /data/local/tmp/FelisTest &&
adb shell '/data/local/tmp/FelisTest; echo "Exit code: $?"'
```

---

# Remote debugging with LLDB

Modern Android NDKs use LLDB for native debugging.

The setup is:

```text
LLDB on Linux
    ↓
ADB port forwarding
    ↓
lldb-server on Android
    ↓
FelisExample / FelisTest running on the phone
```

---

## 9. Copy `lldb-server` to the phone

For an ARM64 device:

```bash
adb push \
"$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/lib/clang/21/lib/linux/aarch64/lldb-server" \
/data/local/tmp/lldb-server
```

Then:

```bash
adb shell chmod +x /data/local/tmp/lldb-server
```

---

## 10. Start `lldb-server`

In terminal 1:

```bash
adb shell '/data/local/tmp/lldb-server platform --listen "*:5039" --server'
```

Leave it running.

---

## 11. Forward the debugger port

In terminal 2:

```bash
adb forward tcp:5039 tcp:5039
```

---

## 12. Start the NDK LLDB client

Use the NDK wrapper:

```bash
"$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/lldb.sh"
```

Inside LLDB:

```text
platform select remote-android
platform connect connect://localhost:5039
platform settings -w /data/local/tmp
```

Load the local Android binary with debug symbols:

```text
file /absolute/path/to/FelisFramework/Build/Android/arm64-v8a/Debug/Example/FelisExample
```

Set a breakpoint and run:

```text
b main
run
```

A successful session should stop on the real Android process at `main()`.

---

## 13. Useful LLDB commands

```text
n                       # next / step over
s                       # step into
c                       # continue
bt                      # backtrace
frame variable          # inspect local variables
p variable              # print expression
b FunctionName          # breakpoint by function
thread list             # list threads
breakpoint list         # list breakpoints
quit                    # exit LLDB
```

---

## 14. Debugging mental model

During remote debugging:

```text
Linux PC:
- LLDB client
- source files
- local Android ELF with debug symbols

Android phone:
- lldb-server
- actual ARM64 process
- threads, stack, heap and execution
```

The program really runs on the phone. LLDB on Linux only controls and inspects it remotely.

---

## 15. Cleanup

Stop `lldb-server` with `Ctrl+C`.

Remove port forwarding:

```bash
adb forward --remove tcp:5039
```

Optionally remove uploaded files:

```bash
adb shell rm /data/local/tmp/lldb-server
adb shell rm /data/local/tmp/FelisTest
adb shell rm /data/local/tmp/FelisExample
```

After development, USB debugging and Samsung Auto Blocker can be restored to their normal settings.

---

## 16. Minimal reminder

If returning to this setup much later:

```bash
source ~/.bashrc
android sdk list
adb devices
cmake --build --preset android-arm64-debug
adb push Build/Android/arm64-v8a/Debug/Test/FelisTest /data/local/tmp/FelisTest
adb shell chmod +x /data/local/tmp/FelisTest
adb shell '/data/local/tmp/FelisTest; echo "Exit code: $?"'
```

For debugging:

```bash
adb shell '/data/local/tmp/lldb-server platform --listen "*:5039" --server'
```

Then in another terminal:

```bash
adb forward tcp:5039 tcp:5039

"$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/lldb.sh"
```

Inside LLDB:

```text
platform select remote-android
platform connect connect://localhost:5039
platform settings -w /data/local/tmp
file /absolute/path/to/FelisFramework/Build/Android/arm64-v8a/Debug/Example/FelisExample
b main
run
```
